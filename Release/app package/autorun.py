import __main__
import os
import arbinrpc

cln=arbinrpc.Client('192.168.23.2',10000,0)


def load_htmls(k):
	__main__.js.set_html(__main__.htmls[k].decode('utf-8'))


def on_html_ready():
	load_htmls('0.html')
	#__main__.js.set_url(r'http://www.baidu.com')
	#pass

def f0_login(usr,pwd):
	if cln.login(usr,pwd):
		load_htmls('1.html')
	else:
		__main__.msgbox('密码错误！')

def on_query(s):
	data= cln.search(s)
	return [x[2:]+x[1:2] for x in data]

n=0
def fun(s):
	global n
	__main__.exe.maindlg.set_title(s+str(n))
	n+=1
	return [n]+['a']*n