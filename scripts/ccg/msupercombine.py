import sys

if __name__ == "__main__":
   if len(sys.argv) != 3:
      print "usage: msuercombine.py msuper_file stagged_file"
      sys.exit(1)
   file = open(sys.argv[1])
   ref = open(sys.argv[2])
   ref_line = ''
   tokens = []
   for line in file:
      line = line.strip()
      if not line: 
         # skip empty
         while ref_line=='':
            ref_line = ref.readline().strip()
         # get tokens
         ref_tokens = [ref_token.split('|') for ref_token in ref_line.split()]
         if len(ref_tokens) != len(tokens): print ref_tokens, '\n', tokens
         assert len(ref_tokens) == len(tokens)
         # add items
         for i in range(len(ref_tokens)):
            assert ref_tokens[i][0] == tokens[i][0]
            if not ref_tokens[i][2] in tokens[i][2:]:
               tokens[i].append(ref_tokens[i][2])
         for token in tokens:
            print " ".join(token); 
         print
         tokens=[]; 
         ref_line = ''
         continue
      item = line.split()
      token = []
      word = item[0]
      pos = item[1]
      number = int(item[2])
      token.append(word)
      token.append(pos)
      for index in range(number):
         token.append(item[2+2*index+1])
      tokens.append(token)
   # job done      
   ref.close()
   file.close() 
