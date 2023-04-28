//
// Created by Zach on 4/27/2023.
//

#include <stdint.h>
#include <windows.h>
#include <Python.h>
#include <stdlib.h>

#ifndef EFTRADAR_POSITION_H
#define EFTRADAR_POSITION_H

public:
    template <typename T>
    T Read(uint64_t address, PyObject *args)
    {
        PyObject *read;

        if (PyArg_ParseTuple(args, "O:read", &temp)) {
            if (!PyCallable_Check(temp)) {  // not callable type
                return NULL;
            }
            Py_XINCREF(read); /* Add a reference to new callback */
        }

        unsigned long size = sizeof(T);

        PyObject *arglist;
        arglist = Py_BuildValue("(Kk)", address, size);
        result = PyObject_CallObject(read, arglist);
        Py_DECREF(arglist);

        return (T)result;
    }


#endif //EFTRADAR_POSITION_H
