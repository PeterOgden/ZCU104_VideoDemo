from pynq import DefaultIP
import os

bistream_path = os.path.join(
    os.path.dirname(os.path.realpath(__file__)),
    "video_demo.bit")

class TestPatternGenerator(DefaultIP):
    bindto = ["xilinx.com:ip:v_tpg:7.0"]
    def __init__(self, description):
        super().__init__(description)
        
    @property
    def height(self):
        return self.read(0x10)
    
    @height.setter
    def height(self, value):
        self.write(0x10, value)
        
    @property
    def width(self):
        return self.read(0x18)
    
    @width.setter
    def width(self, value):
        self.write(0x18, value)
        
    @property
    def pattern(self):
        return self.read(0x20)
    
    @pattern.setter
    def pattern(self, value):
        self.write(0x20, value)
        
    def start(self):
        self.write(0x00, 0x81)
        
    def stop(self):
        self.write(0x00, 0x00)
        
class LogoLayer:
    def __init__(self, array):
        self.array = array
        self.width = 32
        self.height = 32
        self.min_r = 1
        self.max_r = 0
        self.min_g = 1
        self.max_g = 0
        self.min_b = 1
        self.max_b = 0
        self.x = 0
        self.y = 0
        self.alpha = 0
        self.scale = 0
        self._enable_reg = self.array[0x10:0x11]
        self._mask = 1 << 15
        
    @property
    def x(self):
        return self.array[0x400]
    
    @x.setter
    def x(self, value):
        self.array[0x400] = value
        
    @property
    def y(self):
        return self.array[0x402]
    
    @y.setter
    def y(self, value):
        self.array[0x402] = value
    
    @property
    def width(self):
        return self.array[0x404]
    
    @width.setter
    def width(self, value):
        self.array[0x404] = value
        
    @property
    def height(self):
        return self.array[0x406]
    
    @height.setter
    def height(self, value):
        self.array[0x406] = value
        
    @property
    def scale(self):
        return self.array[0x408]
    
    @scale.setter
    def scale(self, value):
        self.array[0x408] = value   
     
    @property
    def alpha(self):
        return self.array[0x40a]
    
    @alpha.setter
    def alpha(self, value):
        self.array[0x40a] = value
        
    @property
    def min_r(self):
        return self.array[0x40c]
    
    @min_r.setter
    def min_r(self, value):
        self.array[0x40c] = value
        
    @property
    def min_g(self):
        return self.array[0x40e]
    
    @min_g.setter
    def min_g(self, value):
        self.array[0x40e] = value
        
    @property
    def min_b(self):
        return self.array[0x410]
    
    @min_b.setter
    def min_b(self, value):
        self.array[0x410] = value
                
    @property
    def max_r(self):
        return self.array[0x412]
    
    @max_r.setter
    def max_r(self, value):
        self.array[0x412] = value
                
    @property
    def max_g(self):
        return self.array[0x414]
    
    @max_g.setter
    def max_g(self, value):
        self.array[0x414] = value
                
    @property
    def max_b(self):
        return self.array[0x416]
    
    @max_b.setter
    def max_b(self, value):
        self.array[0x416] = value
        
    def enable(self):
        current_mask = self._enable_reg[0]
        current_mask |= self._mask
        self._enable_reg[0] = current_mask
    
    def disable(self):
        current_mask = self._enable_reg[0]
        current_mask &= ~self._mask
        self._enable_reg[0] = current_mask
        
class VideoLayer:
    def __init__(self, array, index, enable_reg):
        self._array = array
        self.alpha = 256
        self.x = 0
        self.y = 0
        self.scale = 0
        self.buffer = 0
        self._mask = 1 << index
        self._enable_reg = enable_reg
        
    @property
    def alpha(self):
        return self._array[0]
    
    @alpha.setter
    def alpha(self, value):
        self._array[0] = value
        
    @property
    def x(self):
        return self._array[2]
    
    @x.setter
    def x(self, value):
        self._array[2] = value

    @property
    def y(self):
        return self._array[4]
    
    @y.setter
    def y(self, value):
        self._array[4] = value
        
    @property
    def width(self):
        return self._array[6]
    
    @width.setter
    def width(self, value):
        self._array[6] = value
        
    @property
    def stride(self):
        return self._array[8]
    
    @stride.setter
    def stride(self, value):
        self._array[8] = value
        
    @property
    def height(self):
        return self._array[10]
    
    @height.setter
    def height(self, value):
        self._array[10] = value
        
    @property
    def scale(self):
        return self._array[12]
    
    @scale.setter
    def scale(self, value):
        self._array[12] = value
        
    @property
    def buffer(self):
        return self._array[16]
    
    @buffer.setter
    def buffer(self, value):
        self._array[16] = value    
    
    def enable(self):
        current_mask = self._enable_reg[0]
        current_mask |= self._mask
        self._enable_reg[0] = current_mask
    
    def disable(self):
        current_mask = self._enable_reg[0]
        current_mask &= ~self._mask
        self._enable_reg[0] = current_mask
    
class VideoMixer(DefaultIP):
    bindto = ["xilinx.com:ip:v_mix:3.0"]
    
    def __init__(self, description):
        super().__init__(description)
        array = self.mmio.array
        enable_reg = array[0x10:0x11]
        self.layers = [VideoLayer(array[0x80 + 0x40 * i:0x120 + 0x40 *i], i + 1, enable_reg) for i in range(8)]
        self.logo = LogoLayer(array)
           
    @property
    def height(self):
        return self.read(0x18)
    
    @height.setter
    def height(self, value):
        self.write(0x18, value)
        
    @property
    def width(self):
        return self.read(0x10)
    
    @width.setter
    def width(self, value):
        self.write(0x10, value)
        
    @property
    def background(self):
        return (self.read(0x28), self.read(0x30), self.read(0x38))
    
    @background.setter
    def background(self, value):
        if len(value) != 3:
            raise RuntimeError("Background should be a 3-tuple")
        self.write(0x28, value[0])
        self.write(0x30, value[1])
        self.write(0x38, value[2])
        
    def start(self):
        self.write(0x00, 0x81)
        
    def stop(self):
        self.write(0x00, 0x00)
        
    def enable_layer(self, layer):
        current_mask = self.read(0x40)
        current_mask |= 1 << layer
        self.write(0x40, current_mask)
    
    def disable_layer(self, layer):
        current_mask = self.read(0x40)
        current_mask &= ~(1 << layer)
        self.write(0x40, current_mask)
        
        
class OpticalFlow(DefaultIP):
    bindto = ["xilinx.com:hls:optical_flow:1.0"]
    def __init__(self, description):
        super().__init__(description)
        self.write(0x88, 1080)
        self.write(0x90, 1920)
        self.write(0x28, 0)
        self.write(0x30, 0)
        self.write(0x40, 16)
        self.write(0x48, 10)
        self.write(0x60, 0xFFF2)
        self.write(0x68, 16)
        self.write(0x70, 0)
        self.write(0x80, 0)
        self.write(0x78, 0)
        
    def process(self, frame, prev_frame, out_frame):
        self.write(0x10, frame.physical_address)
        self.write(0x18, prev_frame.physical_address)
        self.write(0x20, out_frame.physical_address)
        self.write(0x00, 0x1)

class FilterPipeline(DefaultIP):
    bindto = ["xilinx.com:hls:filter_pipeline:1.0"]
    def __init__(self, description):
        super().__init__(description)
        self.write(0xa0, 1080)
        self.write(0xa8, 1920)
        self.min_thresh = self.mmio.array[0x20:0x21]
        self.max_thresh = self.mmio.array[0x22:0x23]
        self.colour = self.mmio.array[0x24:0x25]
        self.absolute = self.mmio.array[0x26:0x27]
        self.coeffs = self.mmio.array.view(dtype='i2')[0x20:0x39].reshape((5,5))
        
    def process(self, frame, out_frame):
        self.write(0x10, frame.physical_address)
        self.write(0x18, out_frame.physical_address)
        self.write(0x00, 0x01)