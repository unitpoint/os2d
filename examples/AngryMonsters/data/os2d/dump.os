/******************************************************************************
* Copyright (C) 2014 Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Please feel free to contact me at anytime, 
* my email is evgeniy.golovin@unitpoint.ru, skype: egolovin
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

local function checkProps(val){
	if(val.hasOwnProperty()){ 
		printf("<%s:%d> ", typeOf(val), val.__id)
		return true
	}
}

function dump(val){
	var dump_recurse_check = {}
	function(val, deep, is_key){
		echo is_key ? "  " * deep : ""
		// don't allow userdata, so let's check exact type
		if(typeOf(val) == "object" || checkProps(val)){
			if(val in dump_recurse_check){
				echo "<<RECURSE>>\n"
			}else{
				dump_recurse_check[val] = true
				echo "{\n"
				for(var k, v in val.dumpIter()){
					_F(k, deep+1, true)
					echo " = "
					_F(v, deep+1)
				}
				echo("  " * deep, "}\n")
			}
		}else{
			echo(val, !is_key ? "\n" : "")
		}
	}(val, 0)
}
