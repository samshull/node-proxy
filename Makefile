all:
	@echo "BUILDING: C++ Component"
	@cd src;node-waf configure build;cd ..
	@cp ./src/build/default/node-proxy.node ./lib

clean:
	rm -rf src/build/
	rm -rf src/.lock-wscript
	rm -rf lib/node-proxy.node