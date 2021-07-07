CC=gcc

webmake:
	$(CC) main.c --output webgen
example: webmake
	cp -r example/images out && cp example/style.css out && ./webgen -s style.css -i in -o out -h example/header.html -f example/footer.html -a example/iheader.html -b example/ifooter.html