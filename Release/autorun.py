import __main__
import os

def on_html_ready():
	__main__.js.set_url(os.getcwd()+'\\index.html')
	#__main__.js.set_url(r'http://www.baidu.com')

n=0
def fun(s):
	global n
	__main__.exe.maindlg.set_title(s+str(n))
	n+=1