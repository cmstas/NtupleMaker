#Contains the core CMS2 makers. Does not contain Gen or PAT makers
import FWCore.ParameterSet.Config as cms

from CMS3.NtupleMaker.beamSpotMaker_cfi            import *
from CMS3.NtupleMaker.vertexExtraMaker_cfi         import *
from CMS3.NtupleMaker.luminosityMaker_cfi          import *
from CMS3.NtupleMaker.recoConversionMaker_cfi      import *
from CMS3.NtupleMaker.muonExtraMaker_cfi           import *
from CMS3.NtupleMaker.pftauExtraMaker_cfi          import *
from CMS3.NtupleMaker.photonExtraMaker_cfi         import *
from CMS3.NtupleMaker.muToTrigAssMaker_cfi         import *
from CMS3.NtupleMaker.elToTrigAssMaker_cfi         import *
from CMS3.NtupleMaker.photonToTrigAssMaker_cfi     import *
