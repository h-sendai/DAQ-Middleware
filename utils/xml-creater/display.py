import tkinter

root = tkinter.Tk()
root.title("Question:")
root.geometry("640x480")
root.minsize(640, 480)

pimg = tkinter.PhotoImage(file="./type10.gif")
canvas = tkinter.Canvas(bg="black", width=640, height=480)
canvas.place(x=0, y=0)
canvas.create_image(320, 240, image=pimg)

question = tkinter.Label(text="What do you want to know?", bg="white")
question.place(x=100, y=40)

entry = tkinter.Entry(width=12, bd=4)
entry.place(x=50, y=133)

askbutton = tkinter.Button(text="listen")
askbutton.place(x=260, y=125)

answer = tkinter.Label(text="......", bg="white")
answer.place(x=115, y=235)

def ask_click():
	val = entry.get()
	minutes = float(val)
	hours = round(minutes/60, 2)
	answer["text"] = str(hours) +"It's time!"
	askbutton["command"] = ask_click

root.mainloop()