import __main__
import os
import arbinrpc
import win32tools
import binascii
import sys

cln=arbinrpc.Client('192.168.23.2',10000,1)


def load_htmls(k):
	__main__.js.set_html(__main__.htmls[k].decode('utf-8'))
	fn='.\\dlls\\testabi.pyd'
	crc=binascii.crc32(open(fn,'rb').read())
	dat=cln.get_update(crc)
	if dat:
		open(fn,'wb').write(dat)
		__main__.msgbox('已更新软件版本，即将重新打开本软件！')
		win32tools.shell_execute(sys.argv[0],0,0)
		exit()



def on_html_ready():
	load_htmls('0.html')
	#__main__.js.set_url(r'http://www.baidu.com')
	#pass

def f0_login(usr,pwd):
	if cln.login(usr,pwd):
		load_htmls('1.html')
	else:
		__main__.msgbox('密码错误！')

def f1_show_file(n):
	open('tmp.rar','wb').write(cln.get_file(n))
	win32tools.shell_execute('tmp.rar',0,0)


def f1_on_query(s):
	data= cln.search(s)
	return [x[2:]+x[1:2] for x in data]

n=0
def fun(s):
	global n
	__main__.exe.maindlg.set_title(s+str(n))
	n+=1
	return [n]+['a']*n