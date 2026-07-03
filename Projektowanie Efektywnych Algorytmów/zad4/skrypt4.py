import subprocess
import os
import re
import sys

PROGRAM_EXE = "program.exe" 
CONFIG_FILE = "config.txt"
COSTS_FILE = "optimal_costs.txt"

def update_config(params):
    with open(CONFIG_FILE, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    new_lines = []
    for line in lines:
        updated = False
        for key, value in params.items():
            if line.strip().startswith(key + " =") or line.strip().startswith(key + "="):
                new_lines.append(f"{key} = {value}\n")
                updated = True
                break
        if not updated:
            new_lines.append(line)
            
    with open(CONFIG_FILE, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)

def get_dimension(filepath):
    try:
        with open(filepath, 'r') as f:
            for line in f:
                if "DIMENSION" in line:
                    parts = line.split(':')
                    if len(parts) > 1:
                        return int(parts[1].strip())
    except Exception:
        pass
    return 100 

def run_program_and_get_error():
    try:
        process = subprocess.Popen(
            [PROGRAM_EXE], 
            stdout=subprocess.PIPE, 
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )
        
        full_output = ""
        
        for line in process.stdout:
            sys.stdout.write(line)
            sys.stdout.flush()
            full_output += line
            
        process.wait()
        
        if process.returncode != 0:
            print(f"[BLAD KRYTYCZNY C++] Program wywalil blad: {process.returncode}")
            return 999.0
        
        matches = re.findall(r'Blad:\s*([0-9.]+)\s*\%', full_output)
        if matches:
            return float(matches[-1])
            
        return 999.0 
        
    except FileNotFoundError:
        print(f"[BLAD] Brak pliku {PROGRAM_EXE}.")
        return 999.0
    except Exception as e:
        print(f"[NIEOCZEKIWANY BLAD] {e}")
        return 999.0

def load_optimal_costs(filepath):
    costs = {}
    if not os.path.exists(filepath):
        return costs
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split('=')
            if len(parts) == 2:
                inst_name = parts[0].strip().replace('.atsp', '').replace('.tsp', '')
                costs[inst_name] = int(parts[1].strip())
    return costs

def get_all_instances(base_dir="data"):
    instances = []
    opt_costs = load_optimal_costs(COSTS_FILE)
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith(".tsp") or file.endswith(".atsp"):
                inst_name = file.replace('.atsp', '').replace('.tsp', '')
                if inst_name in opt_costs:
                    instances.append({
                        "name": file,
                        "path": os.path.join(root, file).replace('\\', '/'),
                        "optimum": opt_costs[inst_name]
                    })
    return instances

def main():
    if not os.path.exists(PROGRAM_EXE):
        print(f"[BLAD] Nie ma pliku {PROGRAM_EXE}.")
        sys.exit(1)

    instances = get_all_instances()
    
    instances.sort(key=lambda x: get_dimension(x['path']))

    print("\n=== SKRYPT BADAWCZY GA ===\n")

    for inst in instances:
        N = get_dimension(inst['path'])
        
        tuning_sets = [
            {"POPULATION_SIZE": 50, "MUTATION_RATE": 0.05, "GENERATIONS": 2000, "MUTATION_METHOD": "swap", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 100, "MUTATION_RATE": 0.05, "GENERATIONS": 2000, "MUTATION_METHOD": "swap", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 200, "MUTATION_RATE": 0.05, "GENERATIONS": 2000, "MUTATION_METHOD": "swap", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 100, "MUTATION_RATE": 0.01, "GENERATIONS": 2000, "MUTATION_METHOD": "swap", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 100, "MUTATION_RATE": 0.10, "GENERATIONS": 2000, "MUTATION_METHOD": "swap", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 100, "MUTATION_RATE": 0.10, "GENERATIONS": 2000, "MUTATION_METHOD": "invert", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 100, "MUTATION_RATE": 0.10, "GENERATIONS": 2000, "MUTATION_METHOD": "insert", "CROSSOVER_METHOD": "OX"},
            
            # --- DODANE: Głęboka ewolucja z użyciem INVERT ---
            {"POPULATION_SIZE": 200, "MUTATION_RATE": 0.10, "GENERATIONS": 5000, "MUTATION_METHOD": "invert", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 300, "MUTATION_RATE": 0.15, "GENERATIONS": 10000, "MUTATION_METHOD": "invert", "CROSSOVER_METHOD": "OX"},
            
            # --- Głęboka ewolucja z użyciem INSERT ---
            {"POPULATION_SIZE": 200, "MUTATION_RATE": 0.10, "GENERATIONS": 5000, "MUTATION_METHOD": "insert", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 300, "MUTATION_RATE": 0.15, "GENERATIONS": 10000, "MUTATION_METHOD": "insert", "CROSSOVER_METHOD": "OX"},
            {"POPULATION_SIZE": 500, "MUTATION_RATE": 0.15, "GENERATIONS": 20000, "MUTATION_METHOD": "insert", "CROSSOVER_METHOD": "OX"}
        ]

        if N < 25:
            target_error = 0.0
        elif 25 <= N <= 74:
            target_error = 50.0
        elif 75 <= N <= 449:
            target_error = 100.0
        else: # N >= 450
            target_error = 150.0

        print(f"\n--- BADANIE INSTANCJI: {inst['name']} (Rozmiar: {N}) ---")
        print(f"--- Wymagany blad dla tego rozmiaru: <= {target_error}% ---")
        
        success = False
        for p_set in tuning_sets:
            config_params = {
                "GRAPH_PATH": inst['path'],
                "OPTIMAL_TOUR_COST": inst['optimum'],
                "REPETITIONS": 1,  
                "TIME_LIMIT_SECONDS": 900, 
                **p_set
            }
            update_config(config_params)
            
            error = run_program_and_get_error()
            
            if error <= target_error:
                print(f"[{inst['name']}] CEL OSIAGNIETY (Blad {error}% <= {target_error}%).")
                success = True
                # break 

        if not success:
            print(f"[{inst['name']}] [UWAGA] Po tescie wszystkich wariantow nie udalo sie zejsc do bledu <= {target_error}%.")

if __name__ == "__main__":
    main()
