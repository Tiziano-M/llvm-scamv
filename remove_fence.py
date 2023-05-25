#!/usr/bin/env python3

import sys
import os

import argparse
import logging



parser = argparse.ArgumentParser()
parser.add_argument("binfilename", help="Binary path name", type=str)    
parser.add_argument("-fc", "--fence_config", help="Fence configuration", type=str)
parser.add_argument("-nf", "--number_of_fences", help="Get how many fences there are", action="store_true")
parser.add_argument("-v", "--verbose", help="increase output verbosity", action="store_true")
args = parser.parse_args()


    
def barriers_count(data):
  print(data.count('csdb'))
    
def remove_fence(text):
  lines = text.splitlines()
  data = [l.strip() for l in lines]

  if args.fence_config is None:
    choises = [1]
    for _ in range(data.count('csdb')-1):
      choises.append(0)
  else:
    choises = list(args.fence_config)
  #print(choises)
  new_config = []
  
  new_lines = []
  for line in lines:
    if "csdb" in line:
      if choises != []:
        to_replace = int(not int(choises.pop(0)))
        new_config.append(to_replace)
        if to_replace:
          #logging.debug("replacing fence...")
          new_line = line.replace("csdb", "nop")
          new_lines.append(new_line)
          continue
    new_lines.append(line)

  print(",".join(str(i) for i in new_config))
  
  assert args.binfilename[-2:] == ".S"
  #newbinfile = args.binfilename[:-2]
  with open(args.binfilename, "w") as f:
    f.write("\n".join(new_lines))

def main():
  if args.binfilename:
    with open(args.binfilename, "r") as f:
      data = f.read()
     
  if args.number_of_fences: 
    barriers_count(data)
  else:
    remove_fence(data)



if __name__ == '__main__':
  main()
