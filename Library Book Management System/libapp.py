## Ravish Trivedi
## rvt130130
## CS 6360.002 Database Design homework #3: Library Application
import wx
import sys
import mysql.connector
from random import randint
import datetime
from datetime import timedelta
import time
if len(sys.argv)!=4:
	print "Usage : libapp.py <host address> <database user> <database user password>"
	exit(0)
srcher_id=""
srcher_title=""
srcher_author=""
panel=0
dbhost = str(sys.argv[1])
dbuser = str(sys.argv[2])
dbpasswd = str(sys.argv[3])
class Master(wx.Frame):
	global srcher_id, srcher_title, srcher_author	
	def __init__(self, parent, title):
	        super(Master, self).__init__(parent, title=title, 
	            size=(520, 280), style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX)
	        self.InitUI()
	        self.Centre()
	        self.Show()
    
	def InitUI(self):
	        global panel
	        panel = wx.Panel(self)
	        panel.SetBackgroundColour('#eeffee')
	        panel.SetForegroundColour('#220022')
		self.Bind(wx.EVT_ENTER_WINDOW, self.stats)
	        text1 = wx.StaticText(panel, label="Library Interface", pos=(20, 20))
	        font = wx.Font(24, wx.DECORATIVE, wx.ITALIC, wx.BOLD, underline=True)
	        text1.SetFont(font)
	 	wx.StaticText(panel, label="Search for a book", pos=(20,80))
		wx.StaticText(panel, label="Book Id", pos=(30, 120))
	        self.tc1 = wx.TextCtrl(panel, pos=(110,120), size=(150,20))
	        wx.StaticText(panel, label="Book Title", pos=(30, 150))
	        self.tc2 = wx.TextCtrl(panel, pos=(110,150), size=(150,20))
	        wx.StaticText(panel, label="Author", pos=(30, 180))
	        self.tc3 = wx.TextCtrl(panel, pos=(110,180), size=(150,20))
	        b1 = wx.Button(panel, label='Search', pos=(110, 210) , size=(150,40))
	        b1.Bind(wx.EVT_BUTTON, self.OnButton1Clicked)
		        
		#wx.StaticText(panel, label="Add a new book borrower to database?", pos=(60,330))
		b3 = wx.Button(panel, label='Add User', pos=(370, 20) , size=(120,60))
	        b3.Bind(wx.EVT_BUTTON, self.OnButton3Clicked)

		#wx.StaticText(panel, label="Go to books management ", pos=(60, 268))
		b2 = wx.Button(panel, label='Check In/Out', pos=(370, 90), size=(120,60))
	        b2.Bind(wx.EVT_BUTTON, self.OnButton2Clicked)
		
		b4 = wx.Button(panel, label='Overdue\nBooks', pos=(370, 160), size=(120,60))
	        b4.Bind(wx.EVT_BUTTON, self.OnButton4Clicked)
			
		wx.StaticText(panel, label="Books rented today :", pos=(290, 260))
		self.text2 = wx.StaticText(panel, label="Unavailable", pos=(430, 260))
	
	def stats(self, e):
		conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
        	c = conn.cursor()
		t=datetime.date.today().strftime("%Y-%m-%d")
		sd=datetime.datetime.strptime(t, "%Y-%m-%d")	
		ssd=str(sd)
		ssd=ssd[:10]
		q = "select count(*) from book_loans where date_out = '"+ssd+"';"
		try:
			c.execute(q)
        		trows=c.fetchall()
			self.text2.SetLabel(str(trows[0][0]))
		except mysql.connector.Error as e:
			print str(e)
			self.text2.SetLabel("Unavailable")

	def OnButton4Clicked(self, e):
		Overdue(self, title="Overdue books")
		
	def OnButton1Clicked(self, e):
		global srcher_id, srcher_title, srcher_author	
		srcher_id=str(self.tc1.GetValue())
		srcher_title=str(self.tc2.GetValue())
		srcher_author=str(self.tc3.GetValue())		
		self.search()

	def OnButton2Clicked(self, e):
		Lender(self, title="Check In/Out")	
	
	def OnButton3Clicked(self, e):
	        AddUser(self, title="Add new user")
		#dlg = wx.MessageDialog(self,"Do you really want to close this application?",  "Confirm Exit", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		#result = dlg.ShowModal()
		#dlg.Destroy()
		#if result == wx.ID_OK:
		#	self.Destroy()
	
	def search(self):
	        Search(self,title="Search results")


class Overdue(wx.Frame):
	def __init__(self, parent, title):
	        super(Overdue, self).__init__(parent, title=title, size=(480, 480), style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX)

		self.rows=[]
		self.InitUI()
		self.index=0
		self.list = wx.ListCtrl(panel, pos=wx.Point(100,100), size=wx.Size(-1,100), style=wx.LC_REPORT)
		self.list.InsertColumn(0, 'Book ID', width=110)
		self.list.InsertColumn(1, 'Branch ID', width=80)
		self.list.InsertColumn(2, 'Card No', width=80)
		self.list.InsertColumn(3, 'Date out', width=100)
		self.list.InsertColumn(4, 'Date due', width=100)
		
		for i in range (0,len(self.rows)):
			self.list.InsertStringItem(self.index, self.rows[i][0])
		    	self.list.SetStringItem(self.index, 1, str(self.rows[i][1]))
		    	self.list.SetStringItem(self.index, 2, str(self.rows[i][2]))
			self.list.SetStringItem(self.index, 3, str(self.rows[i][3]))
			self.list.SetStringItem(self.index, 4, str(self.rows[i][4]))
			self.index+=1
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		hbox.Add(self.list, 1, wx.EXPAND)
		panel.SetSizer(hbox)
		self.Centre()
		self.Show()
	
	def InitUI(self):
        	global panel
        	panel = wx.Panel(self)
        	panel.SetBackgroundColour('#ffeeff')
        	panel.SetForegroundColour('#113311')
		conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
        	c = conn.cursor()
		t=datetime.date.today().strftime("%Y-%m-%d")
		sd=datetime.datetime.strptime(t, "%Y-%m-%d")	
		ssd=str(sd)
		ssd=ssd[:10]
		q = "select book_id, branch_id, card_no, date_out, due_date from book_loans where due_date < '"+ssd+"';"
		try:
			c.execute(q)
        		trows=c.fetchall()
			if len(trows)==0:
				error="There are no overdue books"
				self.throw_error(error)
				self.Destroy()
			for row in trows:
				self.rows.append(list(row))
		except mysql.connector.Error as e:
			print str(e)
			self.throw_error(str(e))
			self.Destroy()
		conn.close()

	def OnClose(self, event):
        	del self.rows[:]
		self.Destroy()

	def throw_error(self,error):
		dlg = wx.MessageDialog(self,error,  "Serious Message", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		result = dlg.ShowModal()
		if result == wx.ID_OK:
			ret_val=1
		else:
			ret_val=0
		dlg.Destroy()
		return ret_val
	
class AddUser(wx.Frame):
	def __init__(self, parent, title):
	        super(AddUser, self).__init__(parent, title=title, size=(480, 280), style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX)
		self.InitUI()
		self.Centre()
		self.Show()
	
	def InitUI(self):
        	global panel
        	panel = wx.Panel(self)
        	panel.SetBackgroundColour('#ffeeff')
        	panel.SetForegroundColour('#113311')
		wx.StaticText(panel, label="Enter new user details -", pos=(60, 40))        	
		wx.StaticText(panel, label="Card No.", pos=(60, 70))
	        self.tc1 = wx.TextCtrl(panel, pos=(180,70), size=(90,20))		
		wx.StaticText(panel, label="First Name", pos=(60, 100))
	        self.tc2 = wx.TextCtrl(panel, pos=(180,100), size=(150,20))
		wx.StaticText(panel, label="Last Name", pos=(60, 130))
	        self.tc3 = wx.TextCtrl(panel, pos=(180,130), size=(150,20))
		wx.StaticText(panel, label="Address", pos=(60, 160))
	        self.tc4 = wx.TextCtrl(panel, pos=(180,160), size=(240,20))
		wx.StaticText(panel, label="Pnone No.", pos=(60, 190))
	        self.tc5 = wx.TextCtrl(panel, pos=(180,190), size=(150,20))

		b1 = wx.Button(panel, label='Add User', pos=(180, 220))
	       	b1.Bind(wx.EVT_BUTTON, self.OnButton1Clicked)

	def OnButton1Clicked(self, e):
		self.cid=str(self.tc1.GetValue())
		self.fname=str(self.tc2.GetValue())
		self.lname=str(self.tc3.GetValue())
		self.addr=str(self.tc4.GetValue())
		self.pno=str(self.tc5.GetValue())
		if len(self.cid)==0 or len(self.fname)==0 or len(self.lname)==0 or len(self.addr)==0:
			error="Please complete all information above"
			self.throw_error(error)
			return
		self.validateUser()
	
	def validateUser(self):
		conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
        	c = conn.cursor()
		q = "select * from borrower where card_no="+self.cid
		c.execute(q)
        	trows=c.fetchall()
		print trows		
		if len(trows)!=0:
			error="Card ID already exists! Try another one."
			self.throw_error(error)
			return
		q = "select * from borrower where fname='"+self.fname+"' and lname='"+self.lname+"' and address='"+self.addr+"'"
		c.execute(q)
		trows=c.fetchall()
		print trows
		if len(trows)!=0:
			error="User already exists! Cannot add another account."
			self.throw_error(error)
			return
		q = "insert into borrower values ("+self.cid+",'"+self.fname+"','"+self.lname+"','"+self.addr+"','"+self.pno+"')"
		try:
			c.execute(q)
			conn.commit()
			error="User added to database"
			self.throw_error(error)
		except mysql.connector.Error as e:
			print str(e)
			self.throw_error(str(e))
		conn.close()		
		

	def throw_error(self,error):
		dlg = wx.MessageDialog(self,error,  "Serious Message", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		result = dlg.ShowModal()
		dlg.Destroy()


class Lender(wx.Frame):
	def __init__(self, parent, title):
	        super(Lender, self).__init__(parent, title=title, size=(480, 380), style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX)
		self.InitUI()
		self.Centre()
		self.Show()
	
	def InitUI(self):
        	global panel
        	panel = wx.Panel(self)
        	panel.SetBackgroundColour('#ffeeff')
        	panel.SetForegroundColour('#113311')
		sections=['1','2','3','4','5']
		wx.StaticText(panel, label="Check In a book -", pos=(60, 40))        	
		wx.StaticText(panel, label="Book ID", pos=(60, 70))
	        self.tc1 = wx.TextCtrl(panel, pos=(180,70), size=(150,20))
		wx.StaticText(panel, label="Card ID", pos=(60, 100))
	        self.tc2 = wx.TextCtrl(panel, pos=(180,100), size=(150,20))
		wx.StaticText(panel, label="Borrower Name", pos=(60, 130))
	        self.tc3 = wx.TextCtrl(panel, pos=(180,130), size=(150,20))
		b1 = wx.Button(panel, label='Check In', pos=(180, 160))
	       	b1.Bind(wx.EVT_BUTTON, self.OnButton1Clicked)

		wx.StaticText(panel, label="Check Out a book -", pos=(60, 200))        	
		wx.StaticText(panel, label="Book ID", pos=(60, 230))
	        self.tc4 = wx.TextCtrl(panel, pos=(180,230), size=(150,20))
		wx.StaticText(panel, label="Branch ID", pos=(60, 260))
	        self.tc5 = wx.TextCtrl(panel, pos=(180,260), size=(150,20))
		wx.StaticText(panel, label="Card ID", pos=(60, 290))
	        self.tc6 = wx.TextCtrl(panel, pos=(180,290), size=(150,20))
		b2 = wx.Button(panel, label='Check Out', pos=(180, 320))
	       	b2.Bind(wx.EVT_BUTTON, self.OnButton2Clicked)
	
	def OnButton1Clicked(self, e):
		global inbid,incid,inname
		inbid=str(self.tc1.GetValue())
		incid=str(self.tc2.GetValue())
		inname=str(self.tc3.GetValue())
		if len(inbid)==0 and len(incid)==0 and len(inname)==0:
			error="No data entered!"
			self.throw_error(error)
		else:
			Tlist(self, title="Check In")
	
	def OnButton2Clicked(self, e):
		outbid=str(self.tc4.GetValue())
		outbcid=str(self.tc5.GetValue())
		outcid=str(self.tc6.GetValue())
		if len(outbid)==0 or len(outbcid)==0 or len(outcid)==0:
			error="Please complete all information above to Check Out"
			self.throw_error(error)
		else:
			self.validate_checkOut(outbid,outbcid,outcid)
	
	def validate_checkOut(self,bid,bcid,cid):	
		conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
        	c = conn.cursor()
		q = "select count(*) from book_loans where card_no="+cid
		try:
			c.execute(q)
        		trows=c.fetchall()
		except mysql.connector.Error as e:
			print str(e)
			error="Invalid Card ID"
			self.throw_error(error)
			return 
		if int(trows[0][0])==3:
			error="Cannot rent more than 3 books at a time!"
			self.throw_error(error)
			return
		q = "select no_of_copies from book_copies where book_id='"+bid+"' and branch_id="+bcid
		try:
			c.execute(q)
			trows=c.fetchall()
			if len(trows)==0:
				error="No book with specified ID/branch found!"
				self.throw_error(error)
				return				
			total=int(trows[0][0])
		except mysql.connector.Error as e:
			print str(e)
			error="Database Error :"+str(e)
			self.throw_error(error)
			return
		q = "select count(*) from book_loans where book_id='"+bid+"' and branch_id="+bcid
		try:
			c.execute(q)
			trows=c.fetchall()						
			gone=int(trows[0][0])
		except mysql.connector.Error as e:
			print str(e)
			error="Database Error :"+str(e)
			self.throw_error(error)
			return
		if total<=gone:
			error="No copies of the book are available at selected branch!"
			self.throw_error(error)
			return
		else:
			t=datetime.date.today().strftime("%Y-%m-%d")
			sd=datetime.datetime.strptime(t, "%Y-%m-%d")
			ed= sd + datetime.timedelta(days=14)	
			ssd=str(sd)
			ssd=ssd[:10]
			sse=str(ed)
			sse=sse[:10]		
			q="insert into book_loans values ('"+bid+"',"+bcid+","+cid+",'"+ssd+"','"+sse+"')"
			try:
				c.execute(q)	
				conn.commit()
				error="Book successfully rented :)"
				self.throw_error(error)
			except mysql.connector.Error as e:
				print e
				error="Transaction failed! You already might have rented the same book from this branch. Or card ID is invalid!"
				self.throw_error(error)
		conn.close()
		
	def throw_error(self,error):
		dlg = wx.MessageDialog(self,error,  "Serious Message", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		result = dlg.ShowModal()
		dlg.Destroy()

class Tlist(wx.Frame):
	bid=""
	cid=""
	name=""
	rows=[]
	def __init__(self, parent, title):
	        super(Tlist, self).__init__(parent, title=title, size=(640, 180), style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX)
		self.Bind(wx.EVT_CLOSE, self.OnClose)
		global inbid,incid,inname
		self.bid=inbid
		self.cid=incid
		self.name=inname
		del self.rows[:]
		self.InitUI()
		self.index=0
		self.list = wx.ListCtrl(panel, pos=wx.Point(100,100), size=wx.Size(-1,100), style=wx.LC_REPORT)
		self.list.InsertColumn(0, 'Book ID', width=150)
		self.list.InsertColumn(1, 'Branch ID', width=80)
		self.list.InsertColumn(2, 'Card No', width=80)
		self.list.InsertColumn(3, 'Date out', width=150)
		self.list.InsertColumn(4, 'Date due', width=150)
		
		for i in range (0,len(self.rows)):
			self.list.InsertStringItem(self.index, self.rows[i][0])
		    	self.list.SetStringItem(self.index, 1, str(self.rows[i][1]))
		    	self.list.SetStringItem(self.index, 2, str(self.rows[i][2]))
			self.list.SetStringItem(self.index, 3, str(self.rows[i][3]))
			self.list.SetStringItem(self.index, 4, str(self.rows[i][4]))
			self.index+=1
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		hbox.Add(self.list, 1, wx.EXPAND)
		self.list.Bind(wx.EVT_LIST_ITEM_SELECTED, self.checkIn)
		panel.SetSizer(hbox)
		self.Centre()
		self.Show()
	
	def checkIn(self, e):
		index = self.list.GetFocusedItem()
		error="Are you sure you want to Check In book "+self.rows[index][0]
		ret_val=self.throw_error(error)
		if ret_val==1:
			conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
			c = conn.cursor()
			q = "delete from book_loans where book_id='"+self.rows[index][0]+"' and branch_id="+str(self.rows[index][1])+" and card_no="+str(self.rows[index][2])+";"
			c.execute(q)
			conn.commit()
			conn.close()
			self.list.DeleteItem(index)
			del self.rows[index]
			if len(self.rows)==0:
				self.Destroy()


	def InitUI(self):
        	global panel
        	panel = wx.Panel(self)
        	panel.SetBackgroundColour('#000000')
        	panel.SetForegroundColour('#113311')
		dsq=""
		if len(self.bid)>0:
			dsq+=" and book_id='"+self.bid+"'"
		if len(self.cid)>0:
			dsq+=" and book_loans.card_no="+self.cid
		elif len(self.name)>0:
			dsq+=" and ( fname like '%"+self.name+"%' or lname like '%"+self.name+"%')"
		conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
        	c = conn.cursor()
		q = "select book_id, branch_id, book_loans.card_no, date_out, due_date from book_loans, borrower where book_loans.card_no=borrower.card_no"+dsq
		try:
			c.execute(q)
        		trows=c.fetchall()
			for row in trows:
				self.rows.append(list(row))
		except mysql.connector.Error as e:
			print str(e)
			self.throw_error(str(e))
			self.Destroy()
		conn.close()

	def OnClose(self, event):
        	del self.rows[:]
		self.Destroy()

	def throw_error(self,error):
		dlg = wx.MessageDialog(self,error,  "Serious Message", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		result = dlg.ShowModal()
		if result == wx.ID_OK:
			ret_val=1
		else:
			ret_val=0
		dlg.Destroy()
		return ret_val
	
class Search(wx.Frame):
	global srcher_id, srcher_title, srcher_author
	bid=""
	btitle=""
	bauthor=""
	rows=[]
	sub=[]
	def __init__(self, parent, title):
	        super(Search, self).__init__(parent, title=title, 
			size=(720, 580), style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX)
		self.Bind(wx.EVT_CLOSE, self.OnClose)
	        self.bid=srcher_id
		self.btitle=srcher_title
		self.bauthor=srcher_author
		del self.rows[:]
		del self.sub[:]
		self.InitUI()
		drq=""
		drc=0
		if len(self.bid)>0:
			drq+=" Book ID = "+self.bid
			drc+=1
		if len(self.btitle)>0:
			if drc==1:
				drq+=" and"
			drc+=1
			drq+=" book title like "+self.btitle
		if len(self.bauthor)>0:
			if drc>0:
				drq+=" and"
			drc+=1
			drq+=" book author like "+self.bauthor
		if drc==0:
			drq=" ALL"
		text1 = wx.StaticText(panel, label="Displaying results for"+drq, pos=(30, 20))
	        font = wx.Font(10, wx.DECORATIVE, wx.ITALIC, wx.BOLD, underline=False)
	        text1.SetFont(font)
		if len(self.rows)==0:
			text1 = wx.StaticText(panel, label="NO results to display", pos=(30, 40))
	        	font = wx.Font(14, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, underline=False)
	        	text1.SetFont(font)
		else:
			self.index=0
			self.list = wx.ListCtrl(panel, pos=wx.Point(100,100), size=wx.Size(-1,100), style=wx.LC_REPORT)
			self.list.InsertColumn(0, 'Book ID', width=110)
			self.list.InsertColumn(1, 'Book Title', width=150)
			self.list.InsertColumn(2, 'Book Author', width=150)
			self.list.InsertColumn(3, 'Branch ID', width=80)
			self.list.InsertColumn(4, 'Total Copies', width=100)
			self.list.InsertColumn(5, 'Available', width=100)

			for i in range (0,len(self.rows)):
				self.list.InsertStringItem(self.index, self.rows[i][0])
			    	self.list.SetStringItem(self.index, 1, str(self.rows[i][1]))
				self.list.SetStringItem(self.index, 2, str(self.rows[i][2]))
				self.list.SetStringItem(self.index, 3, str(self.rows[i][3]))
			    	self.list.SetStringItem(self.index, 4, str(self.rows[i][4]))
				lfound=0			
				for r in self.sub:
					if self.rows[i][0]==r[0] and self.rows[i][3]==r[2]:
						lfound=1
						self.rows[i].append(self.rows[i][4]-r[3])					
						self.list.SetStringItem(self.index, 5, str(self.rows[i][5]))
				if lfound==0:
					self.rows[i].append(self.rows[i][4])
					self.list.SetStringItem(self.index, 5, str(self.rows[i][5]))
			    	self.index+=1
			hbox = wx.BoxSizer(wx.HORIZONTAL)
			hbox.Add(self.list, 1, wx.EXPAND)
			panel.SetSizer(hbox)

		self.Centre()
		self.Show()

	def InitUI(self):
        	global panel
        	panel = wx.Panel(self)
        	panel.SetBackgroundColour('#ffeeff')
        	panel.SetForegroundColour('#113311')
        	conn = mysql.connector.connect(host=dbhost, user=dbuser, password=dbpasswd, database='library')
        	c = conn.cursor()
		dsq=""
		if len(self.bid)>1:
			dsq+=" and book.book_id='"+self.bid+"'"
		if len(self.btitle)>1:
			dsq+=" and book.title like '%"+self.btitle+"%'"
		if len(self.bauthor)>1:
			dsq+=" and book_authors.author_name like '%"+self.bauthor+"%'"
		q1="select book.book_id, book.title, book_authors.author_name, branch_id, no_of_copies from book, book_authors, book_copies where book.book_id=book_copies.book_id and book.book_id=book_authors.book_id"+dsq+";"
		q2='select book_loans.book_id, book_loans.date_out, book_loans.branch_id,count(*) from book_loans group by book_loans.book_id, book_loans.branch_id;'
		try:		
			c.execute(q1)
        		trows=c.fetchall()
		except mysql.connector.Error as e:
			error="Database type mismatch Error!"
			self.throw_error(error)
			conn.close()			
			self.Destroy()
		for row in trows:
			self.rows.append(list(row))
		c.execute(q2)
		tsub=c.fetchall()
		for row in tsub:
			self.sub.append(row)
        	#conn.commit()
		conn.close()

	def OnClose(self, event):
        	#dlg = wx.MessageDialog(self,"Do you really want to close this application?",  "Confirm Exit", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		#result = dlg.ShowModal()
		#dlg.Destroy()
		#if result == wx.ID_OK:
		del self.rows[:]
		del self.sub[:]
		self.Destroy()

	def throw_error(self,error):
		dlg = wx.MessageDialog(self,error,  "Serious Message", wx.OK|wx.CANCEL|wx.ICON_QUESTION)
		result = dlg.ShowModal()
		if result == wx.ID_OK:
			ret_val=1
		else:
			ret_val=0
		dlg.Destroy()
		return ret_val

if __name__ == '__main__':  
	app = wx.App()
	Master(None, title='Interface Menu')
	app.MainLoop()
