doxygen:
	doxygen

clean:
	rm -f {*.html,*.css,*.js,*.png}
	rm -Rf search

upload: doxygen
	rsync -avz -e ssh . njh@www.aelius.com:~/public_html/ethersia/

.PHONY: clean doxygen upload
