# Voxel Experiments

## Setup notes

- Everything is expected to be run from the `ROFI_ROOT` folder, because generated paths are relative.
- For setting up Docker, run
```{sh}
docker build . -f experiments/voxel/enviroment.Dockerfile -t voxel-experiments
```

## Generating Tasks

### Generate rofibots each to each tasks:
```sh
experiments/voxel/gen_tasks_args.py e2e -f old \
$(find data/configurations/old/rofibots/ -type f | sort) |
experiments/voxel/gen_tasks_matrix.py - experiments/voxel/args_rofi-voxel.json
```

### Generate tangled to snake tasks:
```sh
experiments/voxel/gen_tasks_args.py snake -f old \
$(find experiments/voxel/snake_10_tangled_10/ -type f | sort) \
$(find experiments/voxel/snake_5_tangled_100/ -type f | sort) |
experiments/voxel/gen_tasks_matrix.py - experiments/voxel/args_rofi-voxel.json
```

## Files

- `args_rofi-voxel.json` - Matrix arguments for `rofi-voxel` command.
- `environment.Dockerfile` - Dockerfile for the experiment. **Make sure the branch is set correctly.**
- `gen_tangled_data.py` - Generates source data by randomly tangling rofi worlds by given number of steps.
- `gen_tasks.py` - Generates task sets. **Make sure to use paths relative to ROFI_ROOT.**
    - e2e - every world to every world (with the same module count)
    - snake - every world to snake (of the same module count)

## Folders

- `snake_5_tangled_100` - Generated by
    ```sh
    experiments/voxel/gen_tangled_data.py $(for i in {2..5}; do echo data/configurations/old/snake/m${i}_snake.rofi; done) --steps 100 --samples 9 -o experiments/voxel/snake_5_tangled_100
    ```
- `snake_10_tangled_10` - Generated by
    ```sh
    experiments/voxel/gen_tangled_data.py $(for i in {2..10}; do echo data/configurations/old/snake/m${i}_snake.rofi; done) --steps 10 --samples 9 -o experiments/voxel/snake_10_tangled_10
    ```

## Requirements

- python modules: prettytable
    - `pip3 install prettytable`