" Vim syntax file to the language developed in INE5426 course.

syntax match Error ".*"
syntax match None ".*"
" This is a kludge to define Error matches only when specified in an nextgroup.
" The first line defines Error as anything, and the second define None also
" as anything. Since Error was defined before None, Vim will give preference
" to the latter.
" This behaviour also allow us to define other syntactic categories that will
" override None.

syntax match Keyword "^f\>"
syntax match Keyword "^fx\>"
syntax match Keyword "^fy\>"
syntax match Keyword "^xf\>"
syntax match Keyword "^yf\>"
syntax match Keyword "^xfx\>"
syntax match Keyword "^yfx\>"
syntax match Keyword "^xfy\>"
syntax match Keyword "^class\>"
syntax match Keyword "^category\>"

syntax match Number "\d\+"
syntax match String "\"\(\\\"\|[^"]\|\n\)*\""

syntax match Include "^include [^ \t\n\r\f\v{},]\+" nextgroup=Error

highlight clear None
