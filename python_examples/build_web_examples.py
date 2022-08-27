"""Script which builds webpages + scripts for the documentation website."""

import argparse
import inspect
import os

import camera_movement
import getting_started
import mesh_animation

ExampleTemplate = """
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="utf-8"/>
    <title>{}</title>
    <script src="scripts/{}.js"></script>
    <link href="styles/theme.css" rel="stylesheet" type="text/css">
</head>

<body>
    <h1>Python Code</h1>
    <pre><code data-language="python">{}</code></pre>
    <script src="scripts/rainbow-custom.min.js"></script>
</body>

</html>
"""


def _parse_args():
    parser = argparse.ArgumentParser("Web example builder")
    parser.add_argument("output_dir")
    return parser.parse_args()


def _prep_code(code: str) -> str:
    lines = code.split("\n")
    # remove function name
    lines = lines[1:]
    # remove indent
    lines = [line[4:] for line in lines]
    return "\n".join(lines)


def _process_example(example, output_dir: str):
    script_path = os.path.join(output_dir, "scripts", "{}.js".format(example.Name))
    scene = example.build_scene()
    scene.save_as_script(script_path, True)
    html_path = os.path.join(output_dir, "{}.html".format(example.Name))
    code = inspect.getsource(example.build_scene)
    code = _prep_code(code)
    html = ExampleTemplate.format(example.Title, example.Name, code)
    with open(html_path, "w") as file:
        file.write(html)


def _main():
    args = _parse_args()
    for example in [camera_movement, getting_started, mesh_animation]:
        _process_example(example, args.output_dir)


if __name__ == "__main__":
    _main()
