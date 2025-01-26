[보고서](https://github.com/min000914/Dining-philosophers-problem/blob/main/Os_Project.pdf)

오퍼레이팅 시스템에서 핵심적인 Problem들 중 Producer – Consumer Problem, Readers- Writers Problem, Dining Philosphers Problem을 다양한 조건에서 해결하기 위하여 문제 해결방법을 설계하였다.

## 개발환경
개발언어: C
Os: Ubuntu 22.04.2 LTS
gccVersion: gcc (Ubuntu 11.3.0-1ubuntu1~22.04.1) 11.3.0
코드편집기: VsCode

## Producer – Consumer Problem
생산자와 소비자의 활동을 관리하는 문제로, 생산자는 Buffer에 생산을 하고, 소비자는 Buffer에서 생산자가 만든 것을 소비한다. 이 때 생산자는 Buffer의 Size를 고려하여, Buffer가 꽉 찼다면 더 이상 생산을 못하도록 하고, 소비자가 Buffer를 비워준다면(Consume) 다시 생산을 하도록 한다. 또한, 소비자는 Buffer가 Empty상태라면, 더 이상 소비를 하지 못하도록 하고, 생산자가 Buffer를 채워준다면(Prodeuce)다시 소비를 하도록 한다.


## Readers- Writers Problem
Writes와 Readers의 활동을 관리하는 문제로, Writer는 특정 변수를 변경하고, Reader는 변수를 출력하도록 한다. 이 때 Writer는 Reader가 변수를 Read하거나 다른 Writer가 변수를 변경하는 도중에는 변수에 접근할 수 없으며, Reader는 Writer가 변수를 변경하는 도중에 변수에 접근하지 못하도록 한다. 하지만 Reader는  변수에 변화를 주지 않으므로 다수의 Reader 변수에 접근할 수 있도록 허용한다.


## Dining Philosphers Problem
원탁에 5명의 철학자와 5개의 포크가 있을 때 철학자가 자신의 양옆에 있는 2개의 포크를 집어야만 식사를 할 수 있다는 문제로, 5명의 철학자가 각자 포크를 하나씩만 들고 있는다면 5명의 철학자 모두 식사를 하지 못하는 상태로 멈춰있게 되고, 이 상태가 유지된다. 이 문제에서 철학자는 Process(혹은 Thread)를 나타내고, 포크는 Process가 필요로하는 공유자원을 나타내며, 5명의 철학자 그 누구도 식사를 하지 못하는 상태는 Process들의 DeadLock 상태를 나타낸다. 이 때 철학자의 행동을 제어해서 DeadLock상태에 빠지지 않도록 하는 문제이다.

