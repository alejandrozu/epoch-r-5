import sys

def convert(dlx_file, sat_file):
    with open(dlx_file, 'r') as f:
        lines = f.read().splitlines()
    
    if not lines: return
    
    header = lines[0].split()
    n_cols = int(header[0])
    n_rows = int(header[1])
    
    covers = [[] for _ in range(n_cols)]
    
    for i in range(1, len(lines)):
        parts = lines[i].split()
        if not parts: continue
        
        # parts are the columns covered by this row
        row_id = i # 1-based
        for col_str in parts:
            col = int(col_str)
            covers[col].append(row_id)
            
    with open(sat_file, 'w') as f:
        # compute number of clauses
        num_clauses = 0
        for col in range(n_cols):
            c_len = len(covers[col])
            num_clauses += 1 + c_len * (c_len - 1) // 2
            
        f.write(f"p cnf {n_rows} {num_clauses}\n")
        
        for col in range(n_cols):
            # At least one
            for x in covers[col]:
                f.write(f"{x} ")
            f.write("0\n")
            
            # At most one
            for a in range(len(covers[col])):
                for b in range(a + 1, len(covers[col])):
                    f.write(f"-{covers[col][a]} -{covers[col][b]} 0\n")

convert('dlx_23_11.txt', 'sat_23_11.cnf')
