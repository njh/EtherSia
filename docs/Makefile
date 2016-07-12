html: doxygen

doxygen:
	@doxygen Doxyfile
	@echo ""
	@echo "Done, error log follows:"
	@echo ""
	@cat "doxygen.log"

clean:
	rm -rf html
	rm -f "doxygen.log"

upload: doxygen
	rsync -avz -e ssh html/ njh@www.aelius.com:~/public_html/ethersia/

.PHONY: html doxygen clean upload
