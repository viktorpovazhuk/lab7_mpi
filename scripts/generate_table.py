import sys, random

def main():
    if len(sys.argv) != 6:
        print("Error! Correct format:\n$ python3 generate_table.py %path/to/table/file% %width% %height% %dx% %dy%")
        exit()
    table_fpath = sys.argv[1]
    width, height, dx, dy = [float(i) for i in sys.argv[2:]]

    num_points_x, num_points_y = int(width / dx), int(height / dy)

    base = 30
    border_base = 50

    table = [[0 for j in range(num_points_x)] for i in range(num_points_y)]
    for i in range(num_points_y):
        for j in range(num_points_x):
            if (i == 0 or j == 0 or i == num_points_y - 1 or j == num_points_x - 1):
                border_base += random.random() - 0.5
                table[i][j] = f"{round(border_base, 4):.4f}"
            else:
                base += random.random() - 0.5
                table[i][j] = f"{round(base, 4):.4f}"
    
    with open(table_fpath, 'w'):
        for i in range(num_points_y):
            print(*table[i])

if __name__ == "__main__":
    main()
