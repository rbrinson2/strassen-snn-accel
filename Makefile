
top_exe = SMM1
source_dir = src
sources = $(source_dir)/SMM1.v $(source_dir)/SMM0.v 
top_module = --top $(top_exe)
synth_dir = synth
synth = results
route_dir = route
testbenches = testbenches/$(top_exe)_tb.cpp

flags = -cc --exe -x-assign fast --trace --build -j 0 

default: testbench
	

testbench:
	@echo "#------------------------------------#"
	@echo "#             TESTBENCH              #"
	@echo "#------------------------------------#"


	# --------------------------------------------- Verilate
	verilator $(flags) $(sources) $(testbenches) $(top_module)	
	mkdir -p logs
	obj_dir/V$(top_exe) +trace

	# --------------------------------------------- GTKWave
	# gtkwave logs/top_dump.vcd

.PHONEY: clean
clean:
	rm -r obj_dir/ logs/

