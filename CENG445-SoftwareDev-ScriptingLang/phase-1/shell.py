import code
import os

def load_models():
    models_path = os.path.join(os.path.dirname(__file__), 'models.py')
    with open(models_path, 'rb') as models_file:
        exec(compile(models_file.read(), models_path, 'exec'))

def main():
    banner = "Python shell for models.py. Type exit() to exit."
    local_vars = globals().copy()  # Copy the current global environment
    load_models()  # Load your models into the environment
    local_vars.update(globals())  # Update the local environment with loaded models

    # Start an interactive console
    code.interact(banner=banner, local=local_vars)

if __name__ == '__main__':
    main()
