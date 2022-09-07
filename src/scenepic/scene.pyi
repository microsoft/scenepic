from typing import Mapping, Union

import numpy as np

from .audio_track import AudioTrack
from .camera import Camera
from .canvas2d import Canvas2D
from .canvas3d import Canvas3D
from .drop_down_menu import DropDownMenu
from .graph import Graph
from .image import Image
from .label import Label
from .mesh import Mesh, MeshUpdate
from .text_panel import TextPanel
from .ui_parameters import UIParameters
from .video import Video


class QuantizationInfo:
    """Information about the results of quantization."""

    @property
    def keyframe_count(self) -> int:
        """The number of keyframes used."""

    @property
    def estimated_size_ratio(self) -> float:
        """The estimated size ratio after compression."""

    @property
    def mean_error(self) -> float:
        """The mean per-frame error."""

    @property
    def max_error(self) -> float:
        """The maximum per-frame error."""


class Scene:
    """Top level container representing an entire ScenePic Scene."""

    def __init__(self, scene_id: str):
        """Constructor.

        Args:
            scene_id (str, optional): a unique identifier for the Scene (will be
                                      automatically populated if not provided). Defaults to None.
        """

    def grid(self, width: str, grid_template_rows: str, grid_template_columns: str) -> None:
        """Defines a CSS layout grid for the canvases of the ScenePic.

        Description:
            Should not be used in conjunction with `html_id` for individual canvases. See 
            https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Grid_Layout for more information.

        Args:
            width (str): CSS declaration for the width of the ScenePic.
            grid_template_rows (str): CSS declaration defining the rows of the ScenePic
            grid_template_columns (str): CSS declaration defining the columns of the ScenePic
        """

    def place(self, canvas_id: str, grid_row: str, grid_column: str) -> None:
        """Places a canvas at a specific place in the ScenePic grid.

        Note:
            For use with Scene.grid
        
        Args:
            canvas_id (str): The unique ID of the canvas to place
            grid_row (str): the CSS row specification for the Canvas
            grid_column (str): the CSS column specification for the Canvas
        """

    def create_canvas_3d(self, canvas_id: str, width: int, height: int,
                         html_id: str, camera: Camera,
                         ui_parameters: UIParameters, media_id: str) -> Canvas3D:
        """Create a new 3D Canvas and append to the Scene.

        Args:
            canvas_id (str, optional): a unique identifier for the Canvas (will be
                                        automatically populated if not provided). Defaults to None.
            width (int, optional): the width in pixels of the Canvas on the HTML page. Defaults to 400.
            height (int, optional): the height in pixels of the Canvas on the HTML page. Defaults to 400.
            html_id (str, optional): id of an HTML element to use as this
                                        Canvas's parent in the HTML DOM (otherwise simply
                                        appended to document). Defaults to None.
            camera (Camera, optional): Camera parameters for the Canvas. See `Camera` for defaults.
            shading (Shading, optional): Set of Shading parameters for the Canvas. See `Shading` for defaults.
            ui_parameters (UIParameters, optional): Set of user interface parameters. See `UIParameters` for
                                                    defaults.
            media_id (str, optional): optional ID of a media file to attach to the canvas. This file will be
                                        used to drive playback, i.e. frames will be displayed in time with
                                        the playback of the media file.

        Returns:
            Canvas3D: a reference to the new Canvas object
        """

    def create_canvas_2d(self, canvas_id: str, width: int, height: int,
                         html_id: str, background_color: np.ndarray,
                         media_id: str) -> Canvas2D:
        """Create a new 2D Canvas and append to the Scene.

        Args:
            canvas_id (str, optional): a unique identifier for the Canvas (will be
                                        automatically populated if not provided). Defaults to None.
            width (int, optional): the width in pixels of the Canvas on the HTML page. Defaults to 400.
            height (int, optional): the height in pixels of the Canvas on the HTML page. Defaults to 400.
            html_id (str, optional): id of an HTML element to use as this
                                        Canvas's parent in the HTML DOM (otherwise simply
                                        appended to document). Defaults to None.
            background_color (np.ndarray, optional): the background color of the canvas. Defaults to Black.
            media_id (str, optional): optional ID of a media file to attach to the canvas. This file will be
                                        used to drive playback, i.e. frames will be displayed in time with
                                        the playback of the media file.

        Returns:
            Canvas2D: a reference to the new Canvas object
        """

    def create_graph(self, canvas_id: str, width: int, height: int,
                     name_align: str, value_align: str,
                     html_id: str, background_color: np.ndarray,
                     margin: Graph.Margin, font_family: str,
                     name_size: float, value_size: float,
                     media_id: str) -> Graph:
        """Create a new Graph canvas and append to the Scene.

        Args:
            canvas_id (str, optional): a unique identifier for the Canvas (will be
                                        automatically populated if not provided). Defaults to None.
            width (int, optional): the width in pixels of the Canvas on the HTML page. Defaults to 400.
            height (int, optional): the height in pixels of the Canvas on the HTML page. Defaults to 400.
            name_align (str, optional): How to align the sparkline label
                                        (one of 'left', 'right', 'top', or 'bottom').
                                        Defaults to 'left'.
            value_align (str, optional): How to align the sparkline value
                                        (one of 'left', 'right', 'top', or 'bottom').
                                        Defaults to 'right'.
            html_id (str, optional): id of an HTML element to use as this
                                        Canvas's parent in the HTML DOM (otherwise simply
                                        appended to document). Defaults to None.
            background_color (np.ndarray, optional): the background color of the canvas. Defaults to White.
            margin (Margin, optional): the outer margin of the graph. Defaults to Margin(10).
            font_family (str, optional): the font family used for the graph labels. Defaults to "sans-serif".
            name_size (float, optional): the text size in pixels used for the graph labels. Defaults to 12.0.
            value_size (float, optional): the text size in pixels used for the graph values. Defaults to 12.0.
            media_id (str, optional): optional ID of a media file to attach to the canvas. This file will be
                                        used to drive playback, i.e. frames will be displayed in time with
                                        the playback of the media file.

        Returns:
            Graph: a reference to the new Graph object
        """

    def create_mesh(self, mesh_id: str, layer_id: str, double_sided: bool,
                    camera_space: bool, shared_color: np.ndarray,
                    texture_id: Union[Image, str], nn_texture: bool,
                    use_texture_alpha: bool, is_billboard: bool,
                    is_label: bool) -> Mesh:
        """Create a new Mesh and append to the Scene.

        Description:
            Meshes are Scene-wide resources that can be reused across multiple
            Frames in multiple Canvas3Ds.

        Args:
            mesh_id (str, optional): a unique identifier for the Mesh (will be
                                    automatically populated if not provided). Defaults to None.
            layer_id (str, optional): an identifier to allow several Meshes to be grouped together
                                    into a "layer".  Layers should be used in conjunction with
                                    Canvas3D.set_layer_settings(). Defaults to None.
            double_sided (bool, optional): whether to turn off back face culling and draw the Mesh's
                                        triangles as double sided. Defaults to False.
            camera_space (bool, optional): whether this Mesh is defined in camera space (cannot
                                        be moved in the ScenePic user interface) or world space (standard).
                                        Defaults to False.
            shared_color (np.ndarray, optional): see `Mesh` constructor.
            texture_id (Union[Image, str], optional): see `Mesh` constructor.
            nn_texture (bool, optional): whether the shader should use nearest-neighbor
                        texture interpolation. Defaults to True.
            use_texture_alpha (bool, optional): whether to use the alpha channel in the texture
                                                for transparency (only relevant for textured Meshes).
                                                Defaults to True.
            is_billboard (bool, optional): draw this Mesh as a billboard (i.e. always facing the user)
                                        rather than rotating with the rest of the world. Defaults to False.
            is_label (bool, optional): this mesh will be treated specially as a label. Not for public use.
                                    Defaults to False.

        Returns:
            Mesh: a reference to the new Mesh object
        """

    def update_mesh(self, base_mesh_id: str, positions: np.ndarray,
                    normals: np.ndarray, colors: np.ndarray,
                    mesh_id: str) -> MeshUpdate:
        """Create a new mesh from an existing one by replacing its vector positions, normals, and/or colors.

        Args:
            base_mesh_id (str): the unique identifier of the original base mesh. Must already have been created.
            positions (np.ndarray, optional): float32 matrix of [N, 3] new positions
            normals (np.ndarray, optional): float32 matrix of [N, 3] new normals.
            colors (np.ndarray, optional): float32 matrix of [N, 3] new colors.
            mesh_id (str, optional): a unique identifier for the new updated Mesh (will be
                                        automatically populated if not provided). Defaults to None.

        Returns:
            MeshUpdate: a reference to the MeshUpdate object
        """

    def update_instanced_mesh(self, base_mesh_id: str, positions: np.ndarray,
                              rotations: np.ndarray, colors: np.ndarray,
                              mesh_id: str) -> MeshUpdate:
        """Create a new mesh from an existing one by replacing its vector positions, normals, and/or colors.

        Args:
            base_mesh_id (str): the unique identifier of the original base mesh. Must already have been created.
            positions (np.ndarray, optional): float32 matrix of [N, 3] new positions
            rotations (np.ndarray, optional): float32 matrix of [N, 4] new rotations.
            colors (np.ndarray, optional): float32 matrix of [N, 3] new colors.
            mesh_id (str, optional): a unique identifier for the new updated Mesh (will be
                                        automatically populated if not provided). Defaults to None.

        Returns:
            MeshUpdate: a reference to the MeshUpdate object
        """

    def update_mesh_positions(self, base_mesh_id: str, positiosn: np.ndarray,
                              mesh_id: str) -> MeshUpdate:
        """Create a new mesh from an existing one by replacing its vector positions and compute normal values.

        Args:
            base_mesh_id (str): the unique identifier of the original base mesh. Must already have been created.
            positions (np.ndarray): float32 matrix of [N, 3] new positions
            mesh_id (str, optional): a unique identifier for the new updated Mesh (will be
                                        automatically populated if not provided). Defaults to None.
        
        Returns:
            MeshUpdate: a reference to the MeshUpdate object
        """

    def create_audio(self, audio_id: str) -> AudioTrack:
        """Create a new AudioTrack object and append it to the Scene.

        Audio tracks are Scene-wide resources that can be attached to multiple canvases.

        Args:
            audio_id (str, optional): a unique identifier for the Audio track (will be
                                        automatically populated if not provided). Defaults to None.

        Returns:
            AudioTrack: a reference to an Audio track object
        """

    def create_video(self, video_id: str) -> Video:
        """Create a new Video object and append it to the Scene.

        Videos are Scene-wide resources that can be attached to multiple canvases.

        Args:
            video_id (str, optional): a unique identifier for the Video (will be
                                        automatically populated if not provided). Defaults to None.

        Returns:
            Video: a reference to an Video object
        """

    def create_image(self, image_id: str) -> Image:
        """Create a new Image and append to the Scene.

        Images are Scene-wide resources that can be reused as textures
        across multiple Meshes and Frame2Ds.

        Args:
            image_id (str, optional): a unique identifier for the Image (will be
                                        automatically populated if not provided). Defaults to None.

        Returns:
            Image: a reference to the Image object
        """

    def create_label(self, label_id: str, text: str, color: np.ndarray,
                     size_in_pixels: float, font_family: str,
                     layer_id: str, horizontal_align: str, vertical_align: str,
                     offset_distance: float, camera_space: bool) -> Label:
        """Create a new Label and append to the Scene.

        Labels are Scene-wide resources that can be reused across
        multiple Meshes.

        Args:
            label_id (str, optional): a unique identifier for the Label (will be
                                        automatically populated if not provided). Defaults to None.
            text (str, optional): the text to use in the label. Defaults to "Text".
            color (np.ndarray, optional): the color to use. Defaults to White.
            size_in_pixels (float, optional): the size in pixels to display the label at. Defaults to 12.0.
            font_family (str, optional): HTML font family name. Defaults to "sans-serif".
            layer_id (str, optional): an identifier to allow several labels to be grouped together
                        into a "layer".  Layers should be used in conjunction with
                        Canvas3D.set_layer_settings(). Defaults to None.
            horizontal_align (str, optional): one of 'left', 'center', 'right'. how to align the
                                label horizontally. Defaults to "left".
            vertical_align (str, optional): one of 'top', 'middle', 'bottom'. how to align the
                            label vertically. Defaults to "middle".
            offset_distance (float, optional): distance in world space to offset the label by when
                                using left/right/top/bottom alignment. Defaults to 0.1.
            camera_space (bool, optional): whether the label should be created in
                                            camera space. Defaults to False.
        
        Returns:
            Label: a reference to the new Label object
        """

    def create_text_panel(self, text_panel_id: str, html_id: str,
                          add_input_box: bool) -> TextPanel:
        """Create a new TextPanel and add directly to the HTML page.

        Args:
            text_panel_id (str, optional): a unique identifier for the TextPanel (will be
                                            automatically populated if not provided). Defaults to None.
            html_id (str, optional): optional id of an HTML element to use as this
                                        TextPanel's parent in the HTML DOM (otherwise will
                                        simply be appended to document). Defaults to None.
            add_input_box (bool, optional): whether to add an input box to the TextPanel. Defaults to False.

        Returns:
            TextPanel: a reference to the new TextPanel object
        """

    def create_drop_down_menu(self, drop_down_menu_id: str, title: str,
                              html_id: str) -> DropDownMenu:
        """Create a new DropDownMenu and add directly to the HTML page.

        Args:
            drop_down_menu_id (str, optional): a unique identifier for the DropDownMenu
                                                (will be automatically populated if not
                                                provided). Defaults to None.
            title (str, optional): a title for the drop down box. Defaults to None.
            html_id (str, optional): optional id of an HTML element to use as this
                                        TextPanel's parent in the HTML DOM (otherwise
                                        will simply be appended to document). Defaults to None.
        Returns:
            DropDownMenu: a reference to the new DropDownMenu object
        """

    def quantize_updates(self, relative_error_threshold: float,
                         absolute_error_threshold: float,
                         base_mesh_id: str,
                         per_frame_range: bool) -> Mapping[str, QuantizationInfo]:
        """Quantize the mesh updates.

        Description:
            Each update will be reduced in size in such a way as to minimize the expected per-value error from quantization.
            The number of keyframes (and thus the level of compression) can be controlled via two
            thresholds: the relative error threshold, and the absolute error threshold. The relative
            threshold is evaluated as a fraction of max(positions) - min(positions). The absolute
            threshold is measured in raw units.
        
            If the relative error threshold has a positive value but the absolute error threshold is negative,
            only the relative threshold will be used. Conversely, if the relative threshold is negative and the
            absolute threshold is positive then only the absolute threshold will be used. If both are positive,
            then they will both be applied. If both are negative, an exception will be raised.
        
            The quantization process will complete in such a way that (quantized - actual).abs().max() < threshold.
            In essence, each x, y, and z coordinate will individually be different by no more than this threshold
            after quantization. Thus, a possible result of quantization will that every frame is a keyframe, i.e. no
            quantization occurs. More typically, however, the algorithm finds a few "keyframe" meshes which
            minimize the expected error across the remaining (quantized) meshes.            

        Args:
            relative_error_threshold (float, optional): the maximum expected error as a multiple of the range of
                                                        values in the base mesh. Defaults to 1e-5.
            absolute_error_threshold (float, optional): the maximum expected error in absolute units. Defaults to -1.0.
            base_mesh_id (str, optional): ID of the base mesh to use as a filter on quantization. Defaults to None.
            per_frame_range (bool, optional): Whether to use the most accurate range per frame, increasing accuracy
                                                but reducing compression. Defaults to True.

        Returns:
            Mapping[str, QuantizationInfo]: information on the per-mesh quantization process
        """

    def measure_command_size(self) -> Mapping[str, int]:
        """Measures the number of bytes used by command type.

        Returns:
            Mapping[str, int]: the number of bytes per command type
        """

    @property
    def framerate(self) -> float:
        """Number of frames to display per second."""

    @property
    def status_bar_visibility(self) -> str:
        """CSS visibility for the status bar."""

    def configure_user_interface(self, ui_parameters: UIParameters) -> None:
        """Set user interface parameters across all Canvases with given UIParameters instance.

        Args:
            ui_parameters (UIParameters): global UI parameters to set
        """

    def simulate_key_presses(self, canvas_id: Union[Canvas3D, Canvas2D, str], *keypresses) -> None:
        """Simulates one or more keypresses to fire off events in the ScenePic UI at startup, e.g. video playback.

        Args:
            self (Scene): self reference
            canvas_id (Union[Canvas3D, Canvas2D, str]): ID of the canvas on which to fire off this
                                                        event.
            keypresses: the key values
        """

    def link_canvas_events(self, *canvases) -> None:
        """Specify that all input events should be linked across the provided set of Canvases.

        Args:
            self (Scene): self reference
            canvases: the canvases which should have linked events
        """

    def clear_script(self) -> None:
        """Call this if you are dumping a ScenePic script in multiple parts.
        This can be used for client-server interactions, or when chunking a large
        visualization into multiple files. Immediately after this, a
        call to get_script() will returnempty.
        """

    def get_json(self) -> str:
        """Returns a JSON-serialized representation of the Scene as a string.

        If you use clear_script() then this will return updates to
        the Scene since thelast call to clear_script().

        Returns:
            str: a JSON string representing the Scene
        """

    def get_script(self) -> str:
        """Returns a JSONP (https://en.wikipedia.org/wiki/JSONP) script representing the Scene.

        If you use clear_script() then this will return updates to
        the Scene since the last call to clear_script().

        Returns:
            str: a JSONP script representing the Scene
        """

    def save_as_json(self, path: str) -> None:
        """To view the JSON, you will need to separately code up the wrapper html and provide the scenepic.min.js library file.
        Alternatively, use save_as_html() to make a self-contained HTML file.

        Args:
            path (str): the path to the file on disk
        """

    def save_as_script(self, path: str, standalone: bool) -> None:
        """Save the scene as a ScenePic script file (JavaScript JSONP format).

        To view the script, you will need to separately code up the wrapper html.
        Alternatively, use save_as_html() to make a self-contained HTML file.

        Args:
            path (str): the path to the file on disk
            standalone (bool): whether to make the script standalone by including the library
        """

    def save_as_html(self, path: str, title: str,
                     head_html: str, body_html: str) -> None:
        """Save the scene as a self-contained html file with no dependencies.

        Args:
            path (str): the path to the file on disk
            title (str, optional): the HTML title. Defaults to "ScenePic".
            head_html (str, optional): the raw HTML to place in the HEAD tag. Defaults to None.
            body_html (str, optional): the raw HTML to place in the BODY tag. Defaults to None.
            script_path (str, optional): desired relative path for the script. A value of None
                                            indicates to embed the script in the HTML page.
                                            Defaults to None.          
            library_path (str, optional): desired relative path for the library. A value of None
                                            indicates to embed the library in the HTML page.
                                            Defaults to None.          
        """

    @property
    def script_cleared(self) -> bool:
        """Whether the script has been cleared."""
