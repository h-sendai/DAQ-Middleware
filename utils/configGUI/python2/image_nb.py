from Tkinter import *
from notebook import *;

class image_nb(notebook):
    def __init__(self, master, side=LEFT):
        notebook.__init__(self, master)

        self.imgs = []
        self.count = 0
        self.choice = IntVar(0)
        if side in (TOP, BOTTOM): self.side = LEFT
        else: self.side = TOP
        self.rb_fr = Frame(master, borderwidth=2, relief=RIDGE, bg='white')
        self.rb_l = Label(self.rb_fr, text="Configuration Selector", fg='black', bg='white')
        self.rb_l.pack(side=TOP, pady=5)

        self.rb_fr.pack(side=side, fill=BOTH, padx=5, pady=5)

        self.screen_fr = Frame(master, borderwidth=3, relief=RIDGE, bg='white')

        self.screen_fr.pack(fill=BOTH, padx=5, pady=5)

    def __call__(self):
        return self.screen_fr

    def add_screen(self, fr, title, img):
        b = Radiobutton(self.rb_fr, text=title, indicatoron=0,image=img,bg='white',
                         variable=self.choice, value=self.count, selectcolor='yellow',
                         command=lambda: self.display(fr))
        b.pack(fill=BOTH, side=self.side, padx=5,pady=5)

        if not self.active_fr:
            fr.pack(fill=BOTH, expand=1, padx=5,pady=5)
            self.active_fr = fr
        self.imgs.append(img)
        self.count += 1

    def display(self, fr):
        self.active_fr.forget()
        fr.pack(fill=BOTH, expand=1)
        self.active_fr = fr
        myfr = self.choice.get()

    def show_active_fr(self):
        return self.choice.get()
