/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "metadata.h"
#include "error.h"
#include "hash.h"
#include "mem.h"

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>

// =============================================================================
struct metadata_entry {
	uint64_t id;
	const char* value;
};

// =============================================================================
static void
ffmpeg_error(int code, const char url[static 1])
{
	char err[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(code, err, sizeof(err));
	warn("%s: %s", url, err);
}

static const AVStream*
find_first_stream(const AVFormatContext ctx[restrict static 1])
{
	AVStream* ret = NULL;

	for (unsigned int i = 0; i < ctx->nb_streams; ++i) {
		register AVStream* cur = ctx->streams[i];
		enum AVMediaType type = cur->codecpar->codec_type;

		// prefer video stream
		if (type == AVMEDIA_TYPE_VIDEO)
			return cur;

		if (!ret && type == AVMEDIA_TYPE_AUDIO)
			ret = cur;
	}

	return ret;
}

void
metadata_init(metadata_list m[static 1])
{
	memset(m, 0, sizeof(*m));
}

void
metadata_free(metadata_list m[static 1])
{
	for (; m->len; --m->len) {
		metadata_entry* entry = &m->entries[m->len - 1];
		xfree((void*)entry->value);
	}
	free_array(m->entries, m->cap);
	metadata_init(m);
}

void
metadata_set(metadata_list m[restrict static 1], uint64_t id, const char value[restrict static 1])
{
	size_t newlen = m->len + 1;
	if (newlen > INT_MAX)
		die("metadata_insert: too many entries");

	grow_array(m->entries, newlen, m->cap);

	// insert sorted
	metadata_entry* cur = &m->entries[m->len++];
	metadata_entry* prev = cur - 1;
	for (; cur > m->entries && prev->id > id; --cur, --prev)
		swap(*cur, *prev);

	if (cur->id == id)
		xfree((void*)cur->value);

	cur->id = id;
	cur->value = xstrdup(value);
}

uint64_t
metadata_insert(metadata_list m[restrict static 1],
                const char name[restrict static 1],
                const char value[restrict static 1])
{
	uint64_t id = strhash(name);
	metadata_set(m, id, value);
	return id;
}

const char*
metadata_get(metadata_list m[static 1], uint64_t id)
{
	register int min = 0;
	register int max = (int)m->len; // len is limited by INT_MAX in `metadata_insert`

	if (max > 0) {
		while (min <= max) {
			int mid = (min + max) >> 1;
			register const metadata_entry* e = &m->entries[mid];

			if (e->id < id)
				min = mid + 1;
			else if (e->id > id)
				max = mid - 1;
			else
				return e->value;
		}
	}

	return NULL;
}

const char*
metadata_find(metadata_list m[restrict static 1], const char name[restrict static 1])
{
	if (!m->len)
		return NULL;
	return metadata_get(m, strhash(name));
}

const char*
metadata_enumerate(metadata_list m[restrict static 1], void* restrict* it)
{
	for (size_t i = (size_t)*it; i < m->len; ++i) {
		const metadata_entry* e = &m->entries[i];
		*it = (void*)(i + 1);
		return e->value;
	}
	*it = NULL;
	return NULL;
}

bool
metadata_read(const char path[restrict static 1],
              metadata_list out[restrict static 1],
              double duration[restrict static 1],
              bool aggressive)
{
	AVFormatContext* ctx = NULL;
	int code;

	if ((code = avformat_open_input(&ctx, path, NULL, NULL)) != 0) {
		ffmpeg_error(code, path);
		return false;
	}

	if (aggressive) {
		if ((code = avformat_find_stream_info(ctx, NULL)) < 0)
			ffmpeg_error(code, path);
	}

	// prefer duration explicitly set in stream if available
	int64_t iduration = ctx->duration;
	const AVStream* stream = find_first_stream(ctx);
	if (stream && (stream->duration != AV_NOPTS_VALUE))
		iduration = stream->duration;

	if (iduration != AV_NOPTS_VALUE)
		*duration = ((double)iduration / AV_TIME_BASE);

	const struct AVDictionaryEntry* tag = NULL;
	while ((tag = av_dict_iterate(ctx->metadata, tag))) {
		if (tag->value)
			metadata_insert(out, tag->key, tag->value);
	}

	avformat_close_input(&ctx);
	return true;
}
