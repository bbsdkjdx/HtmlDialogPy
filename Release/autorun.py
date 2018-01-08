import __main__
import os

def on_html_ready():
	__main__.js.set_url(os.getcwd()+'\\index.html')
	#__main__.js.set_url(r'http://www.baidu.com')

def fun(s):
	__main__.msgbox(s)