"""
TAML (Tab Accessible Markup Language) Parser and Serializer
Version 0.1.0
"""

from .parser import parse, TAMLError
from .serializer import stringify

__version__ = "0.1.0"
__all__ = ["parse", "stringify", "TAMLError"]
