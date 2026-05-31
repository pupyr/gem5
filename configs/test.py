from gem5.components.boards.simple_board import SimpleBoard
from gem5.components.cachehierarchies.classic.private_l1_private_l2_cache_hierarchy import (
    PrivateL1PrivateL2CacheHierarchy,
)
from gem5.components.memory import SingleChannelDDR3_1600
from gem5.components.processors.cpu_types import CPUTypes
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.isas import ISA
from gem5.resources.resource import BinaryResource
from gem5.simulate.simulator import Simulator

cache_hierarchy = PrivateL1PrivateL2CacheHierarchy(
    l1d_size="32KiB", l1i_size="32KiB", l2_size="256KiB"
)

memory = SingleChannelDDR3_1600(size="2GiB")

processor = SimpleProcessor(isa=ISA.X86, cpu_type=CPUTypes.O3, num_cores=2)

board = SimpleBoard(
    clk_freq="3GHz",
    processor=processor,
    memory=memory,
    cache_hierarchy=cache_hierarchy,
)

binary_path = "tests/multi_threaded_binary"
board.set_se_binary_workload(BinaryResource(local_path=binary_path))

simulator = Simulator(board=board)
print("Начало симуляции двухъядерной системы...")
simulator.run()
print("Симуляция успешно завершена!")
