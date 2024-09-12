# Following makefile performs following recipies
# 1. run : runs the vsim from questasim and takes argument as run.do file
# 2. clean : cleans the log files and work folders generated
run:
	vsim -c -do run.do >> log_file.txt
clean:
	> log_files.log |	rm -r work/ |	rm transcript | rm *dll |	rm -r *.o | rm *vstf


