from enum import Enum
from typing import List, Optional
from dataclasses import dataclass, field
from parse_chakra import Node,Attribute,BoolList,Parse
from pathlib import Path

class NodeType(Enum):
    COMP_NODE = 1
    COMM_SEND_NODE = 2
    COMM_RECV_NODE=3
    COMM_COLL_NODE=4
    

class AttributeKey(Enum):
    comm_para = 1
    comm_size = 2
    comm_src = 3
    comm_dst = 4
    comm_type = 5
    involved_dim=6

# 定义占位符
placeholder = 4294967295

def node_to_int_array(node: Node) -> List[int]:
    int_array = []

    # 将 name 转换为 20 个 int 类型
    name_ints = [ord(c) for c in node.name[:20]]  # 截取前20个字符的 ASCII 值
    name_ints += [placeholder] * (20 - len(name_ints))  # 不足 20 个字符则补 placeholder
    int_array.extend(name_ints)

    # 将 type 转换为 int 类型
    node_type = NodeType[node.type].value if node.type in NodeType.__members__ else placeholder
    int_array.append(node_type)

    # 将 id 转换为一个 int 类型
    node_id = int(node.id) if node.id is not None and node.id.isdigit() else placeholder
    int_array.append(node_id)

    # 确保 dataDeps 不是 None
    data_deps = node.dataDeps if node.dataDeps is not None else []
    # 将 data_deps 转换为 10 个 int 类型
    data_deps_ints = [int(dep) if dep.isdigit() else placeholder for dep in data_deps[:10]]
    data_deps_ints += [placeholder] * (10 - len(data_deps_ints))  # 不足 10 个则补 placeholder
    int_array.extend(data_deps_ints)

    # 将 attr 转换为 11 个 int 类型
    attr_ints = []  # 存储所有 AttributeKey 的 11 个 int

    for key in AttributeKey:
            # 查找当前 AttributeKey 是否存在于 node.attr 中
            matching_attr = next((attr for attr in node.attr if attr.name == key.name), None)
            # 如果匹配到 AttributeKey，设置对应的值
            if key.value in {1, 2, 3, 4,5}:  # 对应占用 2 个 int
                key_ints = [placeholder] * 2
                if matching_attr:
                    if matching_attr.boolVal is not None:
                        key_ints[0] = int(matching_attr.boolVal)
                        key_ints[1] = placeholder
                    elif matching_attr.uint32Val is not None:
                        key_ints[0] = matching_attr.uint32Val
                        key_ints[1] = placeholder
                    elif matching_attr.int64Val is not None:
                        key_ints[0] = int(matching_attr.int64Val & 0xFFFFFFFF)  # 低 32 位
                        key_ints[1] = int((matching_attr.int64Val >> 32) & 0xFFFFFFFF)  # 高 32 位
                    elif matching_attr.uint64Val is not None:
                        key_ints[0] = int(matching_attr.uint64Val & 0xFFFFFFFF)  # 低 32 位
                        key_ints[1] = int((matching_attr.uint64Val >> 32) & 0xFFFFFFFF)  # 高 32 位
            elif key.value == 6:  # 对应占用 3 个 int
                key_ints = [placeholder] * 3
                if matching_attr:
                    if matching_attr.boolList is not None:
                        bool_list_vals = [int(val) for val in matching_attr.boolList.values[:3]]  # 最多取前 3 个布尔值
                        for i, val in enumerate(bool_list_vals):
                            key_ints[i] = val
                        for i in range(len(bool_list_vals), 3):  # 不足 3 个用 placeholder 填充
                            key_ints[i] = placeholder

            # 将当前 AttributeKey 的 2/3 个 int 添加到 attr_ints
            attr_ints.extend(key_ints)

    # 将结果添加到 int_array
    int_array.extend(attr_ints)

    # 将 durationMicros 添加到数组末尾
    int_array.append(node.durationMicros)

    return int_array


def int_array_to_node(int_array: List[int]) -> Node:
    # 将 int 数组转换回 Node 对象

    # 1. 解析 name
    name = ''.join([chr(i) for i in int_array[:20] if i != placeholder])

    # 2. 解析 type
    node_type = NodeType(int_array[20]).name if int_array[20] in NodeType._value2member_map_ else 'UNKNOWN'

    # 3. 解析 id
    node_id = str(int_array[21]) if int_array[21] != placeholder else None

    # 4. 解析 dataDeps
    data_deps = [str(i) for i in int_array[22:32] if i != placeholder]

    # 5. 解析 attr
    attrs = []
    attr_start = 32  # 属性值的起始位置
    for key in AttributeKey:
        if key.value in {1, 2, 3, 4}:  # 1234 占用 2 个 int
            key_ints = int_array[attr_start:attr_start + 2]
            attr_start += 2
        elif key.value == 5:  # 5 占用 3 个 int
            key_ints = int_array[attr_start:attr_start + 3]
            attr_start += 3
        else:
            continue

        # 跳过完全是占位符的 AttributeKey
        if all(i == placeholder for i in key_ints):
            continue

        if key.value in {1, 2, 3, 4}:  # 解析 1234
            if key_ints[0] != placeholder and key_ints[1] == placeholder:
                attrs.append(Attribute(name=key.name, uint32Val=key_ints[0]))
            elif key_ints[0] != placeholder and key_ints[1] != placeholder:
                int64_val = (key_ints[1] << 32) | key_ints[0]
                attrs.append(Attribute(name=key.name, int64Val=int64_val))
        elif key.value == 5:  # 解析 5
            bool_list_vals = [bool(val) for val in key_ints if val != placeholder]
            if bool_list_vals:
                attrs.append(Attribute(name=key.name, boolList=BoolList(values=bool_list_vals)))

    # 6. 解析 durationMicros
    duration_micros = int_array[attr_start]

    # 构造 Node 对象
    return Node(
        name=name,
        type=node_type,
        id=node_id,
        dataDeps=data_deps,
        attr=attrs,
        durationMicros=duration_micros
    )


# 测试示例
def test_conversion():
    json_file_path = '/home/zhangran/madrona2/2/madrona_simple_example/scripts/input/npu.1.json'
    nodes = Parse(json_file_path)
    
    if nodes:
        first_node = nodes[1]
        print(f"转换前的 Node 对象: {first_node}")
        int_array = node_to_int_array(first_node)
        print(f"转换后的整数数组: {int_array}")

        node_back = int_array_to_node(int_array)
        print(f"转换回的 Node 对象: {node_back}")

# 执行测试
# test_conversion()

def nodes_to_int_array(json_file_path):
    # json_file_path = '/home/zhangran/madrona2/2/madrona_simple_example/scripts/input/npu.0.json'
    nodes = Parse(json_file_path)
    result=[]
    if nodes:
        for node in nodes:
            result.extend(node_to_int_array(node))
    return result
 
 
def folder_to_int_array(folder_path):
    # folder_path = '/home/zhangran/madrona2/2/madrona_simple_example/scripts/input'
    result=[]
    folder = Path(folder_path)
    file_paths = [str(file) for file in folder.rglob('*') if file.is_file()]
    file_paths = sorted(file_paths, key=lambda x: x)
    for file in file_paths:
        result.append(nodes_to_int_array(file))
    return result
 