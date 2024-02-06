all: cpu sd ram nic floc

cpu:
	$(MAKE) -C cpuEnergyPerf

sd:
	$(MAKE) -C sdEnergyPerf

ram:
	# RAM target builds nothing, handled in install

nic:
	# NIC target builds nothing, handled in install

floc:
	gcc floc.c -o floc

clean:
	$(MAKE) -C cpuEnergyPerf clean
	$(MAKE) -C sdEnergyPerf clean
	rm -f ePerfCPU.out ePerfSD.out sdFeatures.conf ePerfRAM.sh ePerfNIC.sh floc

install:
	mkdir -p /opt/floc
	cp cpuEnergyPerf/ePerfCPU.out /opt/floc/
	cp sdEnergyPerf/ePerfSD.out /opt/floc/
	cp sdEnergyPerf/sdFeatures.conf /opt/floc/
	cp ramEnergyPerf/ePerfRAM.sh /opt/floc/
	cp nicEnergyPerf/ePerfNIC.sh /opt/floc/
	cp floc /opt/floc/
	chmod +x /opt/floc/ePerfRAM.sh /opt/floc/ePerfNIC.sh /opt/floc/floc
	ln -sf /opt/floc/floc /usr/local/bin/floc

uninstall:
	rm -rf /opt/floc
	rm -f /usr/local/bin/floc