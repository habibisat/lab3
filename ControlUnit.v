/* verilator lint_off UNUSED */
module ControlUnit(
    
    input wire [8:0] IR,
    input wire Run,
    input wire Resetn,
    input wire clk,

    output reg a,
    output reg g,
    output reg [3:0] mux,
    output reg alu_op,
    output reg [7:0] registers,
    output reg done,
    output reg IRen
    );

assign IRen = 1;

localparam STATE_IDLE = 3'd0,
    STATE_ALU_T1 = 3'd1, // alu_op = 0
    STATE_ALU_T2 = 3'd2, 
    STATE_ALU_T3 = 3'd3,
    STATE_MV = 3'd4,
    STATE_MVI = 3'd5;

reg [2:0] CurrentState;
reg [2:0] NextState;

reg [2:0] CMD;
reg [2:0] X_Address;
reg [2:0] Y_Address;


// -----------------------------------------------------------------------------
// Conditional State - Transition
// -----------------------------------------------------------------------------

always@ ( posedge clk ) begin
    if ( Resetn || done ) begin
        CurrentState <= STATE_IDLE;
        IRen <=1;
    end
    else begin 
        CurrentState <= NextState;
        IRen <= 0;
    end
end

always@ ( * ) begin
    CMD = IR[6+:3];
    X_Address = IR[3+:3];
    Y_Address = IR[0+:3];
    NextState = CurrentState ;

    // CMD = 000 is move command
    // CMD = 001 is move immediate command
    // CMD = 010 is add command
    // CMD = 011 is substract command

    case ( CurrentState )
        STATE_IDLE : begin
            a = 0;
            g = 0;
            registers = 8'd0;
            alu_op = 0;
            done = 0;
            if (X_Address != 3'd0 && Y_Address != 3'd0) begin 
                if (CMD == 3'b000) NextState = STATE_MV;
                else if (CMD == 3'b011 || CMD == 3'b010) NextState = STATE_ALU_T1;
                else if (CMD == 3'b001) NextState = STATE_MVI;
            end
        end

        STATE_ALU_T1 : begin
            mux = X_Address+1;
            registers[X_Address] = 1;
            a = 1;
            NextState = STATE_ALU_T2;
            done = 0;
        end

        STATE_ALU_T2 : begin
            registers[X_Address] = 0;
            mux = Y_Address + 1;
            registers[Y_Address] = 1;
            alu_op = CMD[0];
            a = 1;
            g = 1;
            NextState = STATE_ALU_T3;
            done = 0;
        end

        STATE_ALU_T3 : begin
            a = 0;
            registers[Y_Address] = 0;
            alu_op = 0;
            mux = 4'd9;
            g = 1;
            registers[X_Address] = 1;
            done = 1;
        end

        STATE_MV : begin
            mux = Y_Address + 4'd1;
            registers[X_Address] = 1;
            registers[Y_Address] = 1;
            done = 1;
        end

        STATE_MVI: begin
            mux=4'd0;
            registers[X_Address] = 1;
            done = 1;
        end

        default : begin
            NextState = STATE_IDLE;
        end
    endcase
end
// -----------------------------------------------------------------------------

endmodule
// -----------------------------------------------------------------------------
