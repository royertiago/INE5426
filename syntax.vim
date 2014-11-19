" Vim syntax file to the language developed in INE5426 course.

syntax match MustBeEmpty "."
syntax match MustBeSpace "\S.*"
syntax match None "."

highlight clear None
highlight link MustBeSpace Error
" This is a trick to define Error matches only when specified in an nextgroup.
" The first line defines Error as anything, and the second define None also
" as anything. Since Error was defined before None, Vim will give preference
" to the latter.
" This behaviour also allow us to define other syntactic categories that will
" override None.
" Note that we must define both Error and None as "." and not as ".*" because
" vim gives priority to items that started the match earlier, so Error would
" override anything that did not start at the beginning of the line.
"
" MustBeSpace is used for eror reporting; see below.
" MustBeSpace and Error will be overriden by None, and will only gain priority
" via nextgroup mechanism. Then, these groups must fail; if they match, an
" red block will be shown to the user.

syntax match Keyword "^f\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^fx\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^fy\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^xf\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^yf\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^xfx\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^yfx\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^xfy\s*\d\+" nextgroup=MustBeSpace
syntax match Keyword "^class\>"
syntax match Keyword "^category\>"

syntax match Number "\d\+"
syntax match String "\"\(\\\"\|[^"]\|\n\)*\""

syntax match Include "^include [^ \t\n\r\f\v{},]\+" nextgroup=Error

syntax match Comment "\v^((include|class|category|xfx|xfy|yfx|xf|yf|fx|fy|f|) )@!.*"
