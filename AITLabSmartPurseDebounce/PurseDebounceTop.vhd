-------------------------------------------------------
-- 
-- This code is used to debounce a button or rotary encoder.
-- It does it by sampling the signal and when it detects
-- a change, it starts a counter for 500uS and if the signal
-- is at the changed state for a certain percentage of 
-- the time then it will output the signal in a different pin.
--
--Note:  Current code works in synthesis and in simulation but not in hardware :C
-------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    --use ieee.std_logic_unsigned.all;
	use IEEE.numeric_std.all;

entity myDebouncer is
    port (
		inputA: in std_logic;
		outputA: out std_logic;
		outputB: out std_logic
    );
end entity;

architecture rtl of myDebouncer is
    signal count :unsigned (15 downto 0) := (others=>'0');
	component OSCH is
		GENERIC (NOM_FREQ: string := "20.46");
		PORT (	STDBY:IN std_logic;
				OSC:OUT std_logic;
				SEDSTDBY:OUT std_logic);
	end component;
	signal stdby_signal : std_logic;
	signal osc_out : std_logic;
	signal sedstby_signal : std_logic;
	signal counterOnOff : std_logic;
	signal testA : std_logic;
	--signal testB : std_logic;
	--signal testC : std_logic;
	--signal testD : std_logic;
	--signal testE : std_logic;
	--signal testF : std_logic;
	--signal testG : std_logic;
	--signal testH : std_logic;
	--signal testI : std_logic;
	--signal testJ : std_logic;
	
	signal IntegretyCounter: unsigned(15 downto 0) := (others=>'0');
	signal IntegretyLimit: unsigned(15 downto 0) := x"1DE7"; --about 3/4 of 500uS
	signal counterTop :unsigned (15 downto 0) := x"27DF"; --about 500uS
	signal counterBottom : unsigned(15 downto 0) := x"0000";
	
	signal counterOnOffB : std_logic;
	signal testAB : std_logic;
	signal IntegretyCounterB: unsigned(15 downto 0) := x"0000";
	signal countB :unsigned (15 downto 0) := (others => '0');
	
	signal buttonOutputA : std_logic;
	signal buttonOutputB : std_logic;
	
	signal oldSignal : std_logic := '0';
	signal newSignal : std_logic := '0';
	signal tempSignal : std_logic := '0';
begin
oscTest: OSCH 
GENERIC MAP( NOM_FREQ => "20.46" )
port map (STDBY => stdby_signal,
							OSC => osc_out,
							SEDSTDBY => sedstby_signal);
							
	outputB <= osc_out;
	
	--the potentiometer is active low.
    process (osc_out, inputA) begin
        if (rising_edge(osc_out)) then
			tempSignal <= oldSignal;
			oldSignal <= newSignal;
			newSignal <= inputA;
			if((oldSignal xor newSignal) = '1') then
				counterOnOff <= '1';
				testA <= inputA;
			end if;
        end if;
		
		if(counterOnOff = '1' and rising_edge(osc_out)) then
			count <= count + 1;
			if(testA = inputA)then
				IntegretyCounter <= IntegretyCounter + 1;
			end if;
			if(count = counterTop) then				
				if(IntegretyCounter > IntegretyLimit) then
					outputA <= inputA;
				end if;
				count <= x"0000";
				counterOnOff <= '0';
				IntegretyCounter <= x"0000";
			end if;
		end if;
    end process;
	
  
end architecture;