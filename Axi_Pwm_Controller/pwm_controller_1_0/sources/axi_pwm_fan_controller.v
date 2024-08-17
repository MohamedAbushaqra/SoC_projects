`timescale 1ns / 1ps

module pwm_fan_control (
    input wire clk,
    input wire rst,
    input wire [7:0] duty_cycle, // the duty cycle determines the value 
    output reg pwm_output
);

    reg [7:0] counter = 0; // 8-bit counter for fan speed control

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            counter <= 0;
            pwm_output <= 0;
        end else begin
            if (counter >= 255)
                counter <= 0;
            else
                counter <= counter + 1;

            pwm_output <= (counter < duty_cycle);
        end
    end

endmodule
