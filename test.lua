local conio = require("conio")

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
conio.getche()
print("\nAnd again :) ...")
while not conio.kbhit() do
    io.write(".")
    os.execute("sleep 1")
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






