

module SMM0_ctrl (
    input clk, rst, load,
    output reg load_TS,
    output reg compute_M,
    output reg compute_C,
    output reg write_out
);

    localparam IDLE = 0, LOAD_TS = 1, MUL_STAGE = 2, ADD_STAGE = 3, OUTPUT_STAGE = 4;

    reg [2:0] current_state;
    reg [2:0] next_state;
    // State transition
    always @(posedge clk) begin
        if (rst)
            current_state <= IDLE;
        else
            current_state <= next_state;
    end

    // Next state logic
    always @(*) begin
        next_state = current_state;
        case (current_state)
            IDLE:         if (load) next_state = LOAD_TS;
            LOAD_TS:      next_state = MUL_STAGE;
            MUL_STAGE:    next_state = ADD_STAGE;
            ADD_STAGE:    next_state = OUTPUT_STAGE;
            OUTPUT_STAGE: next_state = IDLE;
            default : next_state = IDLE;
        endcase
    end

    // Output logic
    always @(*) begin
        load_TS     = (current_state == LOAD_TS);
        compute_M   = (current_state == MUL_STAGE);
        compute_C   = (current_state == ADD_STAGE);
        write_out   = (current_state == OUTPUT_STAGE);
    end
endmodule
