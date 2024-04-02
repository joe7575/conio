/*
MIT License

Copyright (c) 2015-2024 Thiago Adams [thradams]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
local conio = require("conio")

conio.enable_raw_mode()
conio.clrscr()
conio.gotoxy(35,1)
conio.textcolor("YELLOW")
print("#### Conio Lua Demo ####")
conio.reset()

conio.gotoxy(1,3)
print("Version", conio.version())
print("Press any key to continue...")
conio.getch()
print("Press another key...")
while not conio.kbhit() do
    io.write(".")
    conio.msleep(500)
end

-- Does this work on Linux?
conio.setcursortype("NOCURSOR")
print("\nCursor is hidden now. Press any key to show it...")
conio.getch()
conio.setcursortype("NORMALCURSOR")
conio.getch()

conio.textcolor("LIGHTCYAN")
print("Text color is cyan")
conio.textcolor("RED")
print("Text color is red")
conio.textcolor("GREEN")
print("Text color is green")
conio.textcolor("YELLOW")
print("Text color is yellow")

conio.reset()
conio.textbackground("BLUE")
print("Background color is blue")

conio.textbackground("BLACK")
conio.textcolor("WHITE")
conio.reset()

local info = conio.gettextinfo()
print("Cursor position is: ", conio.wherex(), conio.wherey())
print("Cursor position is: ", info.curx, info.cury)
print("Screen size is:     ", info.screenheight, info.screenwidth)
-- Does this work on Linux?
print("Attribute is:       ", info.attribute, info.normattr)

-----------------------------------------------------------------
local function printf(...)
    io.write(string.format(...))
    io.flush()
end

local function __getch()
    if conio.kbhit() then
        return conio.getch()
    end
    return nil
end

local function getch()
    
    local val = __getch()

    if val == 27 then
        local val2 = __getch()
        if val2 == 91 then
            val = 0x80 + __getch()
        end
    end
    return val
end

print("Output keyboard values")
print("Press 'q' to quit")

while true do
    local val = getch()
    local s = ""
	if val == nil then
		conio.msleep(100)
        local x = conio.wherex()
        local y = conio.wherey()
        conio.gotoxy(70, 1)
        printf("x=%d, y=%d    ", x, y)
        io.flush()
        conio.gotoxy(x, y)
    else
        s = string.char(val)
		io.write(s)
        io.flush()
		if s == "q" then
			break
		end
	end
end

conio.disable_raw_mode()



