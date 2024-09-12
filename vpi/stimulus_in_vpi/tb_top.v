module top;

wire [9:0]  count_out;
reg         load_en;
reg  [ 9:0] data_in;
reg         clk;
reg         resetn;

load_counter DUT (.clk        (clk),
                  .resetn     (resetn),
                  .data_in    (data_in),
                  .load_en    (load_en),
                  .count_out  (count_out));
           
initial begin
   clk = 0;
   load_en = 0;
   forever #10 clk = ~clk;
end

always #20 load_en = ~load_en;

/* $stimulus_in :: <"stimulus file"> , <"output log file">, <input variable> */
always @(posedge clk)  $stimulus_in("input_stim.txt", "output_log.txt", data_in);
initial begin
   $monitor("FROM SIM :: DATA_IN = 0x%0h, COUNT_OUT = 0x%0h", data_in, count_out);
   #500 $finish();
end


endmodule // top
