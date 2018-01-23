import __main__
import os

def on_html_ready():
	__main__.js.set_url(os.getcwd()+'\\0.html')
	#__main__.js.set_url(r'http://www.baidu.com')
	#pass

def f0_login(usr,pwd):
	if usr=='user' and pwd=='password':
		__main__.js.set_url(os.getcwd()+'\\1.html')
	else:
		__main__.msgbox('密码错误！')


n=0
def fun(s):
	global n
	__main__.exe.maindlg.set_title(s+str(n))
	n+=1
	return [n]+['a']*n