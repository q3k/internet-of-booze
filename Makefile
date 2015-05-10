.PHONY = test distfiles build

build:
	cp task/flag.txt meta/flag.txt
	mkdir -p resources
	cp task/libc.so resources/

distfiles:
	mkdir -p distfiles
	cp task/* distfiles/

test:
	bash test.sh
	@echo 'Success!'
