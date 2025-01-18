import xml.etree.ElementTree as ET
import glob
import os

proj = "./Australis-Avionics.uvprojx"
core_target = "../Core/Inc"
lib_target = "../Lib/inc"
data_target = "../Data"

def get_includes(target):
    pattern = os.path.join(target, '**')
    return [path for path in glob.glob(pattern, recursive=True) if os.path.isdir(path)]


rtos_includes = [
    '../Middlewares/Third_Party/FreeRTOS/Source/include',
    '../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F',
    '../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS'
]
driver_includes = [
    '../Drivers/CMSIS/Device/ST/STM32F4xx/Include',
    '../Drivers/CMSIS/Include',
    '../Drivers/STM32F4xx_HAL_Driver/Inc'
]
core_includes = get_includes(core_target)
data_includes = get_includes(data_target)
lib_includes = get_includes(lib_target)

relative_paths = [
    os.path.relpath(path, start=os.getcwd())
    for path in core_includes+lib_includes+rtos_includes+driver_includes+data_includes
]

tree = ET.parse(proj)
root = tree.getroot()
xml_includes = root.find('.//VariousControls/IncludePath')

xml_includes.text = ";".join(relative_paths)
tree.write(proj, encoding='UTF-8', xml_declaration=True)
