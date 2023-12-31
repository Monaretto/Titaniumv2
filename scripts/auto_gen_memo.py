#!/usr/bin/python

import argparse
import os
import yaml
from jinja2 import Template

def generate_memory_manager(memory_areas, output_path):
    """
    Generates the C++ code for the memory manager based on the provided memory areas configuration.

    Parameters:
    - memory_areas (dict): Dictionary containing memory area configurations.
    - output_path (str): Path to the output directory for generated C++ files.
    """
    template_path = "scripts/template/memory_manager_template.j2"
    with open(template_path) as file:
        template = Template(file.read())

    cpp_template = template.render(memory_areas=memory_areas)

    with open(os.path.join("lib/HAL/memory/", "MemoryManager.cpp"), "w") as file:
        file.write(cpp_template)

def generate_memory_area(memory_area, output_path):
    """
    Generates the C++ code for a specific memory area based on the provided configuration.

    Parameters:
    - memory_area (dict): Dictionary containing memory area configuration.
    - output_path (str): Path to the output directory for generated C++ files.
    """
    template_path = "scripts/template/memory_area_template.j2"
    with open(template_path) as file:
        template = Template(file.read())

    cpp_template = template.render(memory_area=memory_area)

    with open(os.path.join(output_path, "src", f"{memory_area['classname']}.h"), "w") as file:
        file.write(cpp_template)

def generate_memory_area_enum(memory_areas, output_path):
    """
    Imports memory area configurations from a YAML file.

    Parameters:
    - filepath (str): Path to the YAML configuration file.

    Returns:
    - dict: Dictionary containing memory area configurations.
    """
    template_path = "scripts/template/memory_area_enum_template.j2"
    with open(template_path) as file:
        template = Template(file.read())

    enumerations_template = template.render(memory_areas=memory_areas)

    with open(os.path.join(output_path, "MemoryAreaEnum.h"), "w") as file:
        file.write(enumerations_template)

def generate_memory_area_test(memory_area):
    """
    Generates the C++ test code for a specific memory area based on the provided configuration.

    Parameters:
    - memory_area (dict): Dictionary containing memory area configuration.

    Returns:
    - str: The generated C++ test code.
    """
    template_path = "scripts/template/memory_area_test_template.j2"
    with open(template_path) as file:
        template = Template(file.read())

    test_code = template.render(memory_area=memory_area)
    folder_name = f"test_{memory_area.get('name')}".lower()
    os.makedirs(f"test/{folder_name}/", exist_ok=True)

    with open(os.path.join(f"test/{folder_name}/", f"test_{memory_area['classname']}.cpp"), "w") as file:
        file.write(test_code)

def import_memory_area_def(filepath):
    with open(filepath, "r") as config_file:
        config_data = yaml.safe_load(config_file)

    return config_data

def get_arguments():
    parser = argparse.ArgumentParser(description="Generate C++ code for memory areas based on YAML configuration.")
    parser.add_argument("-i", "--input", required=False, help="Path to the YAML configuration file", default="scripts/memory_areas.yaml")
    parser.add_argument("-o", "--output", help="Output directory for generated C++ files", required=False, default="lib/HAL/memory/MemoryAreas/")

    return parser.parse_args()

def main():
    args = get_arguments()
    input_path, output_path = args.input, args.output

    os.makedirs(output_path, exist_ok=True)
    os.makedirs(os.path.join(output_path, "src"), exist_ok=True)

    memory_areas = import_memory_area_def(input_path)

    generate_memory_manager(memory_areas, output_path)
    generate_memory_area_enum(memory_areas, output_path)

    for _, value in memory_areas.items():
        generate_memory_area(value, output_path)
        generate_memory_area_test(value)

    print("All memory area generated!")

if __name__ == "__main__":
    main()
