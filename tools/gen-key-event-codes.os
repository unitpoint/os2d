require "std"

print "============================"

var buf = Buffer() << <<<===END==="
/*
AUTO-GENERATED FILE. DO NOT MODIFY.
*/

/******************************************************************************
* Copyright (C) 2012-${DateTime.now().format("Y")} Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Latest source code: https://github.com/unitpoint/os2d
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

===END===

function removeComments(s){
	s = s.replace(Regexp("/\/\*.*?\*\//"), "")
	s = s.replace(Regexp("/\/\/.*$/m"), "")
	return s
}

var contents = removeComments(File.readContents("..\..\..\SDL\include\SDL_scancode.h"))
var m = Regexp(`/(SDL_(SCANCODE_[\w\d_]+))\s*=\s*(\d+)/g`).exec(contents)
for(var i, ident in m[1]){
	buf << "{\"${m[2][i]}\", ${ident}},\n"
}
var scancodes = #m[1]

var contents = removeComments(File.readContents("..\..\..\SDL\include\SDL_keycode.h"))
var m = Regexp(`/(SDLK_([\w\d_]+))\s*=/g`).exec(contents)
for(var i, ident in m[1]){
	buf << "{\"KEYCODE_${m[2][i]}\", ${ident}},\n"
}
var keycodes = #m[1]

var m = Regexp(`/(KMOD_([\w\d_]+))\s*=/g`).exec(contents)
for(var i, ident in m[1]){
	buf << "{\"MOD_${m[2][i]}\", ${ident}},\n"
}
var modes = #m[1]

var m = Regexp(`/#define (KMOD_([\w\d_]+))/g`).exec(contents)
for(var i, ident in m[1]){
	buf << "{\"MOD_${m[2][i]}\", ${ident}},\n"
}
modes = modes + #m[1]

var filename = "..\src\KeyboardEventCodes.inc"
File.writeContents(filename, buf)
print "${filename} saved, scancodes: ${scancodes}, keycodes: ${keycodes}, modes: ${modes}\n"
