#!/usr/bin/env python3
"""
对拍脚本：检测 Scheme 解释器输入输出是否一致
用法: python3 check_interpreter.py
"""

import os
import subprocess
import sys
from pathlib import Path

def clean_output(content):
    """清理解释器输出，移除 scm> 提示符"""
    lines = content.split('\n')
    cleaned_lines = []
    for line in lines:
        if line.startswith('scm> '):
            cleaned_line = line[5:]
            if cleaned_line.strip():
                cleaned_lines.append(cleaned_line)
        elif line.strip() and not line.startswith('scm>'):
            cleaned_lines.append(line)
    return '\n'.join(cleaned_lines)

def run_interpreter(input_file, interpreter_path):
    """运行解释器并返回清理后的输出"""
    try:
        result = subprocess.run(
            [interpreter_path], 
            input=open(input_file, 'r').read(),
            capture_output=True,
            text=True,
            timeout=30
        )
        
        if result.returncode != 0:
            return None, f"解释器执行错误: {result.stderr}"
        
        return clean_output(result.stdout), None
    except subprocess.TimeoutExpired:
        return None, "解释器执行超时"
    except Exception as e:
        return None, f"执行异常: {str(e)}"

def compare_outputs(actual, expected):
    """比较实际输出和期望输出"""
    actual_lines = [line.strip() for line in actual.split('\n') if line.strip()]
    expected_lines = [line.strip() for line in expected.split('\n') if line.strip()]
    
    if len(actual_lines) != len(expected_lines):
        return False, f"行数不匹配: 实际 {len(actual_lines)} 行, 期望 {len(expected_lines)} 行"
    
    for i, (actual_line, expected_line) in enumerate(zip(actual_lines, expected_lines)):
        if actual_line != expected_line:
            return False, f"第 {i+1} 行不匹配:\n  实际: {actual_line}\n  期望: {expected_line}"
    
    return True, "输出完全匹配"

def main():
    # 配置路径
    test_dir = Path("/home/logic/scheme-interpreter/2788")
    interpreter_path = "/home/logic/scheme-interpreter/build/code"
    
    # 检查解释器是否存在
    if not os.path.exists(interpreter_path):
        print(f"错误: 解释器不存在 {interpreter_path}")
        sys.exit(1)
    
    # 获取所有测试文件
    input_files = sorted(test_dir.glob("*.in"))
    
    print(f"找到 {len(input_files)} 个测试文件")
    print("=" * 60)
    
    passed = 0
    failed = 0
    skipped = 0
    
    for input_file in input_files:
        test_name = input_file.stem
        output_file = test_dir / f"{test_name}.out"
        
        print(f"测试 {test_name}:", end=" ")
        
        # 检查是否存在对应的 .out 文件
        if not output_file.exists():
            print("跳过 (无对应 .out 文件)")
            skipped += 1
            continue
        
        # 运行解释器
        actual_output, error = run_interpreter(input_file, interpreter_path)
        if error:
            print(f"失败 - {error}")
            failed += 1
            continue
        
        # 读取期望输出
        try:
            with open(output_file, 'r') as f:
                expected_output = f.read().strip()
        except Exception as e:
            print(f"失败 - 无法读取期望输出: {e}")
            failed += 1
            continue
        
        # 比较输出
        match, message = compare_outputs(actual_output, expected_output)
        
        if match:
            print("通过 ✓")
            passed += 1
        else:
            print(f"失败 ✗")
            print(f"  {message}")
            failed += 1
    
    print("=" * 60)
    print(f"测试结果: 通过 {passed}, 失败 {failed}, 跳过 {skipped}")
    print(f"总计: {passed + failed + skipped}")
    
    if failed > 0:
        print(f"\n有 {failed} 个测试失败!")
        sys.exit(1)
    else:
        print("\n所有测试通过! ✓")

if __name__ == "__main__":
    main()
