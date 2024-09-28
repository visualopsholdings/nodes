Feature: Downstream

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"
      And the server has privateKey "sM[0CH.io/U2[jvLMbB@UEU4Z]lTX@EDG2#DD7UK"
      And the server has pubKey "/9AxT<](&2X.HceOX#VoJ.zONJ#=*lgYu8hKgP*4"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "31r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6("
      
      And there are streams:
         | name               | policy |
         | Conversation 1  | p1     |
         | Conversation 2  | p1     |

      And there are sites:
         | headerTitle     | streamBgColor   |
         | Local           | lightgreen      |

	Scenario: Make sure the server has an upstream
	   When she sends streams as "tracy"
      Then she receives 2 streams
