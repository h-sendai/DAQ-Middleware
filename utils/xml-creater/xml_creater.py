import tkinter as tk

class Frame(tk.Frame):
	def __init__(self, master=None):
		super().__init__(self, master)
		self.pack()
		self.create_widgets()
	
	#def create_widgets(self):

def val(event):
	f = Frame()
	f.pack()

def insert_xml():
	version = "<?xml version=\"1.0\"?>"
	configInfo = ["<configInfo>", "</configInfo>"]
	daqOperator = ["<daqOperator>", "</daqOperator>"]
	daqGroup = ["<daqGroup>", "</daqGroup>"]
	components = ["<components>", "</components>"]

	daqGroups = "daqGroup" # +pid_Id
	#<components> elements x12 
	component = "component" #
	hostAddr = "hostAddr" #
	hostPort = "hostPort" #
	instName = "instName" #
	execPath = "execPath" #
	confFile = "confFile" #
	startOrd = "startOrd" #
	inPorts = "inPorts" #
	outPorts = "outPorts" #
	outPort = "outPort" #
	params = "params" #
	param = "param" #
	#</components>



if __name__ == '__main__':
	root = tk.Tk()
	root.title("DAQMW Component XML Creater")
	# Display
	#root.geometry("640x480")
	root.minsize(640, 480)
	
	Button1 = tk.Button(text=u'botton', width = 20)
	Button1.bind("<Button-1>", val)
	Button1.pack(padx=5, pady=5, anchor=tk.W)
	Button2 = tk.Button(text=u'botton', width = 20)
	Button2.bind("<Button-1>", val)
	Button2.pack(padx=5, pady=5, anchor=tk.W)
	
	label_1 = tk.Label(root, text="Name")
	label_2 = tk.Label(root, text="Password")
	entry_1 = tk.Entry(root) # Entry = input field
	entry_2 = tk.Entry(root)

	#label_1.grid(row = 0)
	#label_2.grid(row = 1)

	#entry_1.grid(row=0, column=1)
	#entry_2.grid(row=1, column=1)

	# tkinter mainloop
	root.mainloop()
	root.destroy()