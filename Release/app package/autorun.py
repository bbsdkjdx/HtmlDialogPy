import __main__


def load_htmls(k):
	__main__.js.set_html(__main__.htmls[k].decode('utf-8'))

def on_html_ready():
	load_htmls('0.html')

