.PHONY = test distfiles build

build:
	echo

distfiles:
	mkdir -p distfiles

test:
	bash test.sh
	@echo 'Success!'
