all:
	@echo "BUILDING: C++ Component"
	@rm -rf src/build/
	@rm -rf src/.lock-wscript
	@rm -rf build/Release/nodeproxy.node
	@cd src;node-waf configure build;cd .. 
	@mkdir build/Release
	@cp src/build/*/node-proxy.node build/Release/nodeproxy.node

clean:
	rm -rf src/build/
	rm -rf src/.lock-wscript
	rm -rf build/Release/nodeproxy.node
	
test: all
	node test/test.js
