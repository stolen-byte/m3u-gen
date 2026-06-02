# SPDX-License-Identifier: GPL-3.0-or-later
# frozen_string_literal: true

require 'asciidoctor'
require 'asciidoctor/extensions'
require 'asciidoctor/converter/manpage'

# ==============================================================================
module J2l
	class ManConverter < Asciidoctor::Converter::ManPageConverter
		extend Asciidoctor::Converter::Config

		register_for 'manpage'

		# The default converter outputs '\f(CR' for text enclosed in backticks '`'
		# which is commonly used for code/verbatim text, we change that to '\fB' here
		# because that is more traditional (ymmv).
		def convert_inline_quoted(node)
			case node.type
			when :monospaced
				%(<#{ESC_BS}fB>#{node.text}</#{ESC_BS}fP>)
			else
				super
			end
		end
	end
end

# ==============================================================================
# Asciidoctor::Extensions.register do
# end
