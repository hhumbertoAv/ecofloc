all: cpu sd ram nic floc

cpu:
	$(MAKE) -C cpuEnergyPerf
	cp cpuEnergyPerf/ePerfCPU.out ./

sd:
	$(MAKE) -C sdEnergyPerf
	cp sdEnergyPerf/ePerfSD.out ./
	cp sdEnergyPerf/sdFeatures.conf ./

ram:
	cp ramEnergyPerf/ePerfRAM.sh ./
	chmod +x ePerfRAM.sh

nic:
	cp nicEnergyPerf/ePerfNIC.sh ./
	chmod +x ePerfNIC.sh

floc:
	gcc floc.c -o floc

clean:
	$(MAKE) -C cpuEnergyPerf clean
	$(MAKE) -C sdEnergyPerf clean
	rm -f ePerfCPU.out ePerfSD.out sdFeatures.conf ePerfRAM.sh ePerfNIC.sh floc
